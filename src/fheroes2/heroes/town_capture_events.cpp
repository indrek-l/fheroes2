/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2026                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "town_capture_events.h"

#include <cstdint>
#include <memory>
#include <vector>

#include "artifact.h"
#include "audio_manager.h"
#include "castle.h"
#include "color.h"
#include "dialog.h"
#include "heroes.h"
#include "kingdom.h"
#include "m82.h"
#include "map_format_info.h"
#include "maps_fileinfo.h"
#include "players.h"
#include "resource.h"
#include "screen.h"
#include "settings.h"
#include "skill.h"
#include "ui_dialog.h"
#include "ui_text.h"
#include "world.h"

void fireTownCaptureEvents( const Castle & castle, Heroes & hero, const bool aiCapturer )
{
    std::vector<Maps::Map_Format::TownCaptureEvent> * eventList = world.getTownCaptureEvents( castle.GetIndex() );
    if ( eventList == nullptr || eventList->empty() ) {
        return;
    }

    const PlayerColorsSet humans = Players::HumanColors();
    const PlayerColor heroColor = hero.GetColor();

    for ( Maps::Map_Format::TownCaptureEvent & event : *eventList ) {
        // The masks were normalised against the map's authorised colours at load time. Here we
        // intersect with the running game's actual human/AI assignment so the right side fires.
        const PlayerColorsSet activeHumanColors = humans & event.humanPlayerColors;
        const PlayerColorsSet activeComputerColors = ( ~humans ) & event.computerPlayerColors;
        const PlayerColorsSet allowed = activeHumanColors | activeComputerColors;

        if ( !( allowed & heroColor ) ) {
            continue;
        }

        // Apply rewards. Order mirrors the placed-event handler so that hero state changes happen
        // before the optional message dialog renders any updated stats.
        const Funds fundsToUpdate = Resource::CalculateEventResourceUpdate( hero.GetKingdom().GetFunds(), event.resources );

        if ( event.resources.GetValidItemsCount() ) {
            hero.GetKingdom().AddFundsResource( event.resources );
        }

        Artifact eventArtifact( event.artifact );
        if ( event.artifact == Artifact::SPELL_SCROLL ) {
            eventArtifact.SetSpell( event.artifactMetadata );
        }

        const Skill::Secondary skill( event.secondarySkill, event.secondarySkillLevel );
        bool willTeachSkill = false;
        if ( skill.isValid() ) {
            if ( hero.HasSecondarySkill( skill.Skill() ) ) {
                willTeachSkill = ( hero.GetSecondarySkills().GetLevel( skill.Skill() ) < skill.Level() );
            }
            else {
                willTeachSkill = !hero.HasMaxSecondarySkill();
            }
        }

        if ( aiCapturer ) {
            // No dialog for AI: silently apply non-UI effects and move on.
            if ( eventArtifact.isValid() ) {
                hero.PickupArtifact( eventArtifact );
            }

            if ( willTeachSkill ) {
                hero.LearnSkill( skill );
                if ( skill.Skill() == Skill::Secondary::SCOUTING ) {
                    hero.Scout( hero.GetIndex() );
                }
            }

            if ( event.experience > 0 ) {
                hero.IncreaseExperience( static_cast<uint32_t>( event.experience ) );
            }
        }
        else {
            // Build the dialog elements so the human player sees what the capture awarded.
            const std::vector<fheroes2::ResourceDialogElement> resourceUI = fheroes2::getResourceDialogElements( fundsToUpdate );

            std::vector<const fheroes2::DialogElement *> elementUI;
            elementUI.reserve( resourceUI.size() );
            for ( const fheroes2::ResourceDialogElement & element : resourceUI ) {
                elementUI.emplace_back( &element );
            }

            std::unique_ptr<fheroes2::ArtifactDialogElement> artifactUI;
            if ( eventArtifact.isValid() ) {
                artifactUI = std::make_unique<fheroes2::ArtifactDialogElement>( eventArtifact );
                AudioManager::PlaySound( M82::TREASURE );
                elementUI.emplace_back( artifactUI.get() );
            }

            std::unique_ptr<fheroes2::SecondarySkillDialogElement> secondarySkillUI;
            if ( willTeachSkill ) {
                secondarySkillUI = std::make_unique<fheroes2::SecondarySkillDialogElement>( skill, hero );
                elementUI.emplace_back( secondarySkillUI.get() );

                hero.LearnSkill( skill );
                if ( skill.Skill() == Skill::Secondary::SCOUTING ) {
                    hero.Scout( hero.GetIndex() );
                    hero.ScoutRadar();
                }
            }

            std::unique_ptr<fheroes2::ExperienceDialogElement> experienceUI;
            if ( event.experience > 0 ) {
                experienceUI = std::make_unique<fheroes2::ExperienceDialogElement>( event.experience );
                elementUI.emplace_back( experienceUI.get() );
            }

            const fheroes2::Text emptyText;
            const fheroes2::Text body( event.message, fheroes2::FontType::normalWhite(), Settings::Get().getCurrentMapInfo().getSupportedLanguage() );

            int32_t dialogHeight = fheroes2::getDialogHeight( emptyText, body, Dialog::OK, elementUI );
            const int32_t displayHeight = fheroes2::Display::instance().height();
            if ( dialogHeight > displayHeight && !elementUI.empty() ) {
                std::vector<const fheroes2::DialogElement *> overflowElements;
                while ( dialogHeight > displayHeight && !elementUI.empty() ) {
                    overflowElements.push_back( elementUI.back() );
                    elementUI.pop_back();
                    dialogHeight = fheroes2::getDialogHeight( emptyText, body, Dialog::OK, elementUI );
                }

                fheroes2::showMessage( emptyText, body, Dialog::OK, elementUI );
                fheroes2::showMessage( emptyText, emptyText, Dialog::OK, overflowElements );
            }
            else {
                fheroes2::showMessage( emptyText, body, Dialog::OK, elementUI );
            }

            // PickupArtifact handles bag-full and magic-book conflicts and shows its own dialog.
            // It must follow the reward dialog so the player sees the reward visual first.
            if ( eventArtifact.isValid() ) {
                hero.PickupArtifact( eventArtifact );
            }

            if ( event.experience > 0 ) {
                hero.IncreaseExperience( static_cast<uint32_t>( event.experience ) );
            }
        }

        // Single-shot semantics: clear both colour masks so a future capture by any player skips this entry.
        if ( !event.isRecurringEvent ) {
            event.humanPlayerColors = 0;
            event.computerPlayerColors = 0;
        }
    }
}
