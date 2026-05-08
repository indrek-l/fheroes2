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

#include "editor_map_events_list_window.h"

#include <cassert>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include "agg_image.h"
#include "cursor.h"
#include "dialog.h"
#include "editor_event_details_window.h"
#include "game_hotkeys.h"
#include "icn.h"
#include "image.h"
#include "interface_list.h"
#include "localevent.h"
#include "map_format_info.h"
#include "math_base.h"
#include "screen.h"
#include "settings.h"
#include "translations.h"
#include "ui_button.h"
#include "ui_dialog.h"
#include "ui_text.h"
#include "ui_window.h"

namespace
{
    const int32_t elementOffset{ 9 };

    const fheroes2::Size eventsArea{ 500, 315 };

    const int32_t listAreaHeightDeduction{ 8 };

    class EventListBox final : public Interface::ListBox<Maps::Map_Format::AdventureMapEventMetadata>
    {
    public:
        using Interface::ListBox<Maps::Map_Format::AdventureMapEventMetadata>::ActionListDoubleClick;
        using Interface::ListBox<Maps::Map_Format::AdventureMapEventMetadata>::ActionListSingleClick;
        using Interface::ListBox<Maps::Map_Format::AdventureMapEventMetadata>::ActionListPressRight;

        EventListBox( const fheroes2::Point & pt, const fheroes2::SupportedLanguage language )
            : ListBox( pt )
            , _language( language )
        {
            // Do nothing.
        }

        void RedrawItem( const Maps::Map_Format::AdventureMapEventMetadata & event, int32_t posX, int32_t posY, bool current ) override
        {
            std::string label = _( "Event %{index}" );
            // ListBox doesn't expose the item's index to RedrawItem so we cannot show it directly here.
            // The label still distinguishes the focused row visually via colour.
            label.replace( label.find( "%{index}" ), 8, std::string{} );

            const fheroes2::FontType font = current ? fheroes2::FontType::normalYellow() : fheroes2::FontType::normalWhite();

            fheroes2::MultiFontText text;

            if ( !event.message.empty() ) {
                fheroes2::Text msg{ '\"' + event.message + '\"', font, _language };
                msg.fitToOneRow( eventsArea.width - 10 );
                text.add( std::move( msg ) );
            }
            else {
                text.add( fheroes2::Text{ _( "(no message)" ), font } );
            }

            text.draw( posX + 5, posY + 5, fheroes2::Display::instance() );
        }

        void RedrawBackground( const fheroes2::Point & /*unused*/ ) override
        {
            _listBackground->restore();
        }

        void ActionCurrentUp() override
        {
            // Do nothing.
        }

        void ActionCurrentDn() override
        {
            // Do nothing.
        }

        void ActionListDoubleClick( Maps::Map_Format::AdventureMapEventMetadata & /*unused*/ ) override
        {
            _isDoubleClicked = true;
        }

        bool isDoubleClicked() const
        {
            return _isDoubleClicked;
        }

        void resetDoubleClickedState()
        {
            _isDoubleClicked = false;
        }

        void ActionListSingleClick( Maps::Map_Format::AdventureMapEventMetadata & /*unused*/ ) override
        {
            // Do nothing.
        }

        void ActionListPressRight( Maps::Map_Format::AdventureMapEventMetadata & /*unused*/ ) override
        {
            // Do nothing.
        }

        void initListBackgroundRestorer( fheroes2::Rect roi )
        {
            _listBackground = std::make_unique<fheroes2::ImageRestorer>( fheroes2::Display::instance(), roi.x, roi.y, roi.width, roi.height );
        }

    private:
        std::unique_ptr<fheroes2::ImageRestorer> _listBackground;

        bool _isDoubleClicked{ false };

        const fheroes2::SupportedLanguage _language;
    };
}

namespace Editor
{
    bool openMapEventsListWindow( std::vector<Maps::Map_Format::AdventureMapEventMetadata> & events, const PlayerColorsSet humanPlayerColors,
                                  const PlayerColorsSet computerPlayerColors, const fheroes2::SupportedLanguage language )
    {
        const CursorRestorer cursorRestorer( true, Cursor::POINTER );

        fheroes2::Display & display = fheroes2::Display::instance();
        fheroes2::StandardWindow background{ eventsArea.width + 50, eventsArea.height + 100, true, display };

        const fheroes2::Rect windowArea{ background.activeArea() };

        int32_t offsetY = windowArea.y + elementOffset;

        const fheroes2::Text title( _( "Tile events:" ), fheroes2::FontType::normalYellow() );
        title.draw( windowArea.x + ( windowArea.width - title.width() ) / 2, offsetY, display );

        offsetY += title.height() + elementOffset;

        const fheroes2::Rect eventsRoi{ windowArea.x + elementOffset, offsetY, eventsArea.width, eventsArea.height };
        background.applyTextBackgroundShading( eventsRoi );

        EventListBox eventList( eventsRoi.getPosition(), language );
        eventList.initListBackgroundRestorer( eventsRoi );

        eventList.SetAreaItems( { eventsRoi.x, eventsRoi.y, eventsRoi.width, eventsRoi.height - listAreaHeightDeduction } );

        const bool isEvilInterface = Settings::Get().isEvilInterfaceEnabled();

        int32_t scrollbarOffsetX = eventsRoi.x + eventsRoi.width + 5;
        background.renderScrollbarBackground( { scrollbarOffsetX, eventsRoi.y, eventsRoi.width, eventsRoi.height }, isEvilInterface );

        const int listIcnId = isEvilInterface ? ICN::SCROLLE : ICN::SCROLL;
        const int32_t topPartHeight = 19;
        ++scrollbarOffsetX;

        eventList.SetScrollButtonUp( listIcnId, 0, 1, { scrollbarOffsetX, eventsRoi.y + 1 } );
        eventList.SetScrollButtonDn( listIcnId, 2, 3, { scrollbarOffsetX, eventsRoi.y + eventsRoi.height - 15 } );
        eventList.setScrollBarArea( { scrollbarOffsetX + 2, eventsRoi.y + topPartHeight, 10, eventsRoi.height - 2 * topPartHeight } );
        eventList.setScrollBarImage( fheroes2::AGG::GetICN( listIcnId, 4 ) );
        eventList.SetAreaMaxItems( 10 );
        eventList.SetListContent( events );
        eventList.updateScrollBarImage();

        eventList.Redraw();

        const int minibuttonIcnId = isEvilInterface ? ICN::CELLWIN_EVIL : ICN::CELLWIN;

        const fheroes2::Sprite & buttonImage = fheroes2::AGG::GetICN( minibuttonIcnId, 13 );
        const int32_t buttonWidth = buttonImage.width();
        const int32_t buttonStrideX = ( eventsArea.width - 5 * buttonWidth ) / 4;

        // Five buttons evenly distributed: add, edit, delete, move-up, move-down.
        // Up/Down reuse the scrollbar arrow icns since fheroes2 has no dedicated reorder sprites.
        fheroes2::Button buttonAdd( eventsRoi.x, eventsRoi.y + eventsRoi.height + 5, minibuttonIcnId, 13, 14 );
        buttonAdd.draw();

        fheroes2::Button buttonEdit( eventsRoi.x + ( buttonWidth + buttonStrideX ), eventsRoi.y + eventsRoi.height + 5, minibuttonIcnId, 15, 16 );
        buttonEdit.draw();

        fheroes2::Button buttonDelete( eventsRoi.x + 2 * ( buttonWidth + buttonStrideX ), eventsRoi.y + eventsRoi.height + 5, minibuttonIcnId, 17, 18 );
        buttonDelete.draw();

        fheroes2::Button buttonUp( eventsRoi.x + 3 * ( buttonWidth + buttonStrideX ), eventsRoi.y + eventsRoi.height + 5, listIcnId, 0, 1 );
        buttonUp.draw();

        fheroes2::Button buttonDown( eventsRoi.x + 4 * ( buttonWidth + buttonStrideX ), eventsRoi.y + eventsRoi.height + 5, listIcnId, 2, 3 );
        buttonDown.draw();

        // Prepare OKAY and CANCEL buttons and render their shadows.
        fheroes2::Button buttonOk;
        fheroes2::Button buttonCancel;

        background.renderOkayCancelButtons( buttonOk, buttonCancel );

        display.render( background.totalArea() );

        bool isRedrawNeeded = false;

        LocalEvent & le = LocalEvent::Get();
        while ( le.HandleEvents() ) {
            buttonOk.drawOnState( le.isMouseLeftButtonPressedAndHeldInArea( buttonOk.area() ) );
            buttonCancel.drawOnState( le.isMouseLeftButtonPressedAndHeldInArea( buttonCancel.area() ) );
            buttonAdd.drawOnState( le.isMouseLeftButtonPressedAndHeldInArea( buttonAdd.area() ) );
            buttonEdit.drawOnState( le.isMouseLeftButtonPressedAndHeldInArea( buttonEdit.area() ) );
            buttonDelete.drawOnState( le.isMouseLeftButtonPressedAndHeldInArea( buttonDelete.area() ) );
            buttonUp.drawOnState( le.isMouseLeftButtonPressedAndHeldInArea( buttonUp.area() ) );
            buttonDown.drawOnState( le.isMouseLeftButtonPressedAndHeldInArea( buttonDown.area() ) );

            if ( le.MouseClickLeft( buttonCancel.area() ) || Game::HotKeyPressEvent( Game::HotKeyEvent::DEFAULT_CANCEL ) ) {
                break;
            }

            if ( buttonOk.isEnabled() && ( le.MouseClickLeft( buttonOk.area() ) || Game::HotKeyPressEvent( Game::HotKeyEvent::DEFAULT_OKAY ) ) ) {
                return true;
            }

            eventList.QueueEventProcessing();

            if ( eventList.IsNeedRedraw() ) {
                eventList.Redraw();
                isRedrawNeeded = true;
            }

            if ( le.MouseClickLeft( buttonAdd.area() ) ) {
                Maps::Map_Format::AdventureMapEventMetadata temp;
                if ( eventDetailsDialog( temp, humanPlayerColors, computerPlayerColors, language ) ) {
                    events.emplace_back( std::move( temp ) );

                    eventList.updateScrollBarImage();
                    eventList.Redraw();
                }

                isRedrawNeeded = true;
            }
            else if ( eventList.isDoubleClicked() || le.MouseClickLeft( buttonEdit.area() ) ) {
                if ( eventList.getCurrentId() < 0 ) {
                    continue;
                }

                eventList.resetDoubleClickedState();

                Maps::Map_Format::AdventureMapEventMetadata temp = eventList.GetCurrent();
                if ( eventDetailsDialog( temp, humanPlayerColors, computerPlayerColors, language ) ) {
                    eventList.GetCurrent() = std::move( temp );

                    eventList.Redraw();
                }

                isRedrawNeeded = true;
            }
            else if ( le.MouseClickLeft( buttonDelete.area() ) ) {
                if ( eventList.getCurrentId() < 0 ) {
                    continue;
                }

                eventList.RemoveSelected();

                eventList.updateScrollBarImage();
                eventList.Redraw();
                isRedrawNeeded = true;
            }
            else if ( le.MouseClickLeft( buttonUp.area() ) ) {
                const int currentId = eventList.getCurrentId();
                if ( currentId > 0 && static_cast<size_t>( currentId ) < events.size() ) {
                    std::swap( events[currentId], events[currentId - 1] );
                    eventList.SetCurrent( static_cast<size_t>( currentId - 1 ) );
                    eventList.Redraw();
                    isRedrawNeeded = true;
                }
            }
            else if ( le.MouseClickLeft( buttonDown.area() ) ) {
                const int currentId = eventList.getCurrentId();
                if ( currentId >= 0 && static_cast<size_t>( currentId + 1 ) < events.size() ) {
                    std::swap( events[currentId], events[currentId + 1] );
                    eventList.SetCurrent( static_cast<size_t>( currentId + 1 ) );
                    eventList.Redraw();
                    isRedrawNeeded = true;
                }
            }
            else if ( le.isMouseRightButtonPressedInArea( buttonCancel.area() ) ) {
                fheroes2::showStandardTextMessage( _( "Cancel" ), _( "Exit this menu without doing anything." ), Dialog::ZERO );
            }
            else if ( le.isMouseRightButtonPressedInArea( buttonOk.area() ) ) {
                fheroes2::showStandardTextMessage( _( "Okay" ), _( "Click to save the events." ), Dialog::ZERO );
            }
            else if ( le.isMouseRightButtonPressedInArea( buttonAdd.area() ) ) {
                fheroes2::showStandardTextMessage( _( "Add Event" ), _( "Add an event to this tile. Events fire in list order when a hero steps on the tile." ), Dialog::ZERO );
            }
            else if ( le.isMouseRightButtonPressedInArea( buttonEdit.area() ) ) {
                fheroes2::showStandardTextMessage( _( "Edit Event" ), _( "Edit the selected event." ), Dialog::ZERO );
            }
            else if ( le.isMouseRightButtonPressedInArea( buttonDelete.area() ) ) {
                fheroes2::showStandardTextMessage( _( "Delete Event" ), _( "Delete the selected event." ), Dialog::ZERO );
            }
            else if ( le.isMouseRightButtonPressedInArea( buttonUp.area() ) ) {
                fheroes2::showStandardTextMessage( _( "Move Up" ), _( "Move the selected event one position earlier in the firing order." ), Dialog::ZERO );
            }
            else if ( le.isMouseRightButtonPressedInArea( buttonDown.area() ) ) {
                fheroes2::showStandardTextMessage( _( "Move Down" ), _( "Move the selected event one position later in the firing order." ), Dialog::ZERO );
            }

            if ( isRedrawNeeded ) {
                isRedrawNeeded = false;

                display.render( windowArea );
            }
        }

        return false;
    }
}
