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

#pragma once

#include <cstdint>
#include <vector>

#include "color.h"

namespace Maps::Map_Format
{
    struct AdventureMapEventMetadata;
}

namespace fheroes2
{
    enum class SupportedLanguage : uint8_t;
}

namespace Editor
{
    struct TriggerHeroOption;

    // List editor for ordered event lists. Used both for placed-event tiles and town-capture
    // events on castles. The isTownCaptureEvent flag adjusts the window title and per-event
    // labelling so the editor reads "Town events" rather than "Tile events" for towns.
    // heroOptions seeds the per-event trigger-hero picker; an empty vector hides the picker.
    bool openMapEventsListWindow( std::vector<Maps::Map_Format::AdventureMapEventMetadata> & events, const PlayerColorsSet humanPlayerColors,
                                  const PlayerColorsSet computerPlayerColors, const fheroes2::SupportedLanguage language, const bool isTownCaptureEvent,
                                  const std::vector<TriggerHeroOption> & heroOptions );
}
