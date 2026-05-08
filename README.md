# fheroes2

**fheroes2** is a recreation of the Heroes of Might and Magic II game engine.

This open source multiplatform project, written from scratch, is designed to reproduce the original game with significant
improvements in gameplay, graphics and logic (including support for high-resolution graphics, improved AI, numerous fixes
and user interface improvements), breathing new life into one of the most addictive turn-based strategy games.

You can find a complete list of all of our changes and enhancements in [**this page**](docs/GAME_IMPROVEMENTS.md).

## This fork

This fork extends the upstream fheroes2 engine with additional scenario-authoring tooling on top of
[**ihhub/fheroes2**](https://github.com/ihhub/fheroes2). All new behavior is gated by `.fh2m` map
format version 17 and savegame format version 1154, so existing maps and saves continue to load
unchanged.

* **Custom victory and defeat epilogues.** A new EPILOGUES button on the map specs window lets the
  map author write their own text for the end-of-scenario message, replacing the engine's generated
  default. Leaving the field empty falls back to the original default.
* **Multiple events per placed-event tile.** Event tiles now hold an ordered list of events with
  add / edit / delete / move-up / move-down controls. Each matching entry fires in editor list
  order; legacy single-event maps load as one-element lists.
* **Town capture events.** Towns and castles get an EVENTS button in their editor properties that
  authors a list of events fired when an enemy hero captures the town. Effects (resources, artifact,
  primary/secondary skills, monsters, message) apply to the capturing hero. AI captures fire the
  same events but suppress the message dialog.
* **Hero-specific trigger filter.** Placed events and town capture events can optionally be
  restricted to a single specific hero. The picker lists placed heroes and heroes locked inside
  Prison objects (the latter become valid triggers once freed). Tile coordinates disambiguate
  unnamed entries.

The branch carrying these changes is
[**scenario-events-phase-1-2**](https://github.com/indrek-l/fheroes2/tree/scenario-events-phase-1-2).

<p align="center">
    <img src="docs/images/screenshots/screenshot_world_map.webp" width="820" alt="Screenshot of the world map">
</p>

<p align="center">
    <img src="docs/images/screenshots/screenshot_battle.webp" width="410" alt="Screenshot of the battle screen">
    <img src="docs/images/screenshots/screenshot_castle.webp" width="410" alt="Screenshot of the castle screen">
</p>

## Download and Install

Please follow the [**installation guide**](docs/INSTALL.md) to download and install fheroes2.

[![Github Downloads](https://img.shields.io/github/downloads/ihhub/fheroes2/total.svg)](https://github.com/ihhub/fheroes2/releases)

## Copyright

All rights for the original game and its resources belong to former The 3DO Company. These rights were transferred to Ubisoft.
We do not encourage and do not support any form of illegal usage of the original game. We strongly advise to purchase the original
game on [**GOG**](https://www.gog.com) or [**Ubisoft Store**](https://store.ubi.com) platforms. Alternatively, you can download a
free demo version of the game. Please refer to the [**installation guide**](docs/INSTALL.md) for more information.

## License

This project is licensed under the [**GNU General Public License v2.0**](https://github.com/ihhub/fheroes2/blob/master/LICENSE).

Initially, the project was developed on [**sourceforge**](https://sourceforge.net/projects/fheroes2/).

## Contribution and Development

This repository is a place for everyone. If you want to contribute, please read more to learn [**how you can contribute**](docs/FAQ.md#q-how-can-i-contribute-to-the-project).

### Developing fheroes2 engine

To build the project from source, please follow [**this guide**](docs/DEVELOPMENT.md).

[![Build Status](https://github.com/ihhub/fheroes2/actions/workflows/push.yml/badge.svg)](https://github.com/ihhub/fheroes2/actions)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=ihhub_fheroes2&metric=bugs)](https://sonarcloud.io/dashboard?id=ihhub_fheroes2)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=ihhub_fheroes2&metric=code_smells)](https://sonarcloud.io/dashboard?id=ihhub_fheroes2)
[![Duplicated Lines (%)](https://sonarcloud.io/api/project_badges/measure?project=ihhub_fheroes2&metric=duplicated_lines_density)](https://sonarcloud.io/dashboard?id=ihhub_fheroes2)

To assist with the graphical asset efforts of the project, please look at our [**graphical artist guide**](docs/GRAPHICAL_ASSETS.md).

If you would like to help translating the project, please read the [**translation guide**](docs/TRANSLATION.md).

### Developing fheroes2 documentation site

To build the [website](https://ihhub.github.io/fheroes2/) from source, please follow
[**this guide**](docs/WEBSITE_LOCAL_DEV.md).

## Donation

We accept donations via [**Patreon**](https://www.patreon.com/fheroes2), [**PayPal**](https://www.paypal.com/paypalme/fheroes2) or [**Boosty**](https://boosty.to/fheroes2).
All donations will be used only for the future project development as we do not consider this project as a source of income by any means.

[![Patreon Donate](https://img.shields.io/badge/Donate-Patreon-green.svg)](https://www.patreon.com/fheroes2)
[![Paypal Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/paypalme/fheroes2)
[![Boosty Donate](https://img.shields.io/badge/Donate-Boosty-green.svg)](https://boosty.to/fheroes2)

## Contacts

Follow us on social networks: [**Facebook**](https://www.facebook.com/groups/fheroes2) or [**VK**](https://vk.com/fheroes2).
We also have a [**Discord**](https://discord.gg/xF85vbZ) server to discuss the development of the project.

[![Facebook](https://img.shields.io/badge/Facebook-blue.svg)](https://www.facebook.com/groups/fheroes2)
[![VK](https://img.shields.io/badge/VK-blue.svg)](https://vk.com/fheroes2)
[![Discord](https://img.shields.io/discord/733093692860137523.svg?label=&logo=discord&logoColor=ffffff&color=7389D8&labelColor=6A7EC2)](https://discord.gg/xF85vbZ)

## Frequently Asked Questions (FAQ)

You can find answers to the most commonly asked questions on our [**F.A.Q. page**](docs/FAQ.md).
