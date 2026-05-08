/***************************************************************************
 *   fheroes2: https://github.com/ihhub/fheroes2                           *
 *   Copyright (C) 2024 - 2025                                             *
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

#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "color.h"

namespace Maps::Map_Format
{
    struct AdventureMapEventMetadata;
    struct MapFormat;
}

namespace fheroes2
{
    enum class SupportedLanguage : uint8_t;
}

namespace Editor
{
    // Selectable hero option for the optional trigger-hero filter on placed and town capture events.
    // uid is the editor-time object UID of a placed hero or a hero locked inside a Prison object;
    // label is the human-readable list entry shown in the picker (with a "(jailed)" suffix where applicable).
    struct TriggerHeroOption
    {
        uint32_t uid{ 0 };
        std::string label;
        bool isJailed{ false };
    };

    // Walks the map for placed-hero and Prison objects and returns one TriggerHeroOption per hero,
    // in the order they appear by tile index.
    std::vector<TriggerHeroOption> buildTriggerHeroOptions( const Maps::Map_Format::MapFormat & map );

    // isTownCaptureEvent retitles the dialog and rewrites the recurring-event tooltip so the
    // editor reads "Town event" / "captures the town" instead of the placed-event wording.
    // heroOptions populates the optional trigger-hero picker; an empty vector hides the picker.
    bool eventDetailsDialog( Maps::Map_Format::AdventureMapEventMetadata & eventMetadata, const PlayerColorsSet humanPlayerColors,
                             const PlayerColorsSet computerPlayerColors, const fheroes2::SupportedLanguage language, const bool isTownCaptureEvent,
                             const std::vector<TriggerHeroOption> & heroOptions );
}
