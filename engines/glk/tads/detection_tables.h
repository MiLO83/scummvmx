/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "engines/game.h"
#include "common/gui_options.h"
#include "common/language.h"

namespace Glk {
namespace TADS {

const PlainGameDescriptor TADS2_GAME_LIST[] = {
	{ "tads2", "TADS 2 Game" },

	// English games
	{ "1893", "1893: A World's Fair Mystery" },
	{ "2044", "2044: Space Troopers" },
	{ "3", "+= 3: A Logical Adventure" },
	{ "aayela", "Aayela, an Interactive Vision" },
	{ "aboveandbeyond", "Above and Beyond!" },
	{ "alienabduction", "Alien Abduction" },
	{ "allalone", "All Alone" },
	{ "amissville", "Amissville" },
	{ "amoi", "A Matter of Importance" },
	{ "anightguest", "A Night Guest" },
	{ "arrival", "Arrival, or Attack of the B-Movie Cliches" },
	{ "asimpletheft", "A Simple Theft" },
	{ "asugaredpill", "A Sugared Pill" },
	{ "awe", "At Wit's End" },
	{ "babel", "Babel" },
	{ "badmachine", "Bad Machine" },
	{ "ballymunadv", "Ballymun Adventure" },
	{ "beetmonger", "The Beetmonger's Journal" },
	{ "bmiss", "Bmissfille" },
	{ "brokenstring", "The Broken String" },
	{ "castleofdoom", "Castle of Doom" },
	{ "coffeequest", "CoffeeQuest" },
	{ "commonground", "Common Ground" },
	{ "ceruleanstowaway", "Cerulean Stowaway" },
	{ "coasthouse", "The Coast House" },
	{ "dampcamp", "The Damp Camp" },
	{ "deanna", "Deanna" },
	{ "deepspace", "Deep Space" },
	{ "depravitybites", "Depravity Bites" },
	{ "diemasse", "Die Vollkommene Masse" },
	{ "ditchday", "Ditch Day Drifter" },
	{ "drool", "Drool" },
	{ "e14s", "Exactly 14 syllables... er, gulps!" },
	{ "ecdysis", "Ecdysis" },
	{ "edgarhuntly", "Edgar Huntly, Or, Memoirs of a Sleepwalker" },
	{ "enhanced", "Enhanced" },
	{ "escapecrazyplace", "Escape from the Crazy Place" },
	{ "excuseme", "Excuse Me, Do You Have The Time?" },
	{ "exhibition", "Exhibition" },
	{ "eterna", "Eterna Corp" },
	{ "findingmartin", "Finding Martin" },
	{ "firebird", "Firebird" },
	{ "firewitch", "John's Fire Witch" },
	{ "fms", "Frobozz Magic Support" },
	{ "foom", "FooM - the shoot-'em up text adventure" },
	{ "fourin1", "Four in One, an Interactive Marx Brothers Comedy" },
	{ "fren51", "The Frenetic Five vs. Sturm und Drang" },
	{ "fren52", "The Frenetic Five vs. Mr. Redundancy Man" },
	{ "fren53", "The Frenetic Five vs. The Seven Deadly Dwarves" },
	{ "friendlyfoe", "Friendly Foe" },
	{ "frustration", "Frustration" },
	{ "ftf", "First Things First" },
	{ "fusillade", "Fusillade" },
	{ "galacticpeace", "Galactic Peace" },
	{ "gc", "GC: A Thrashing Parity Bit of the Mind" },
	{ "gift3", "Gifts of Phallus 3: The Valkin War," }, 
	{ "gna", "Generic New York Apartment Building" },
	{ "goldenfleece", "The Golden Fleece" },
	{ "goldenskull", "The Golden Skull" },
	{ "hamper", "To Hell in a Hamper" },
	{ "helpfulman", "The Adventures of Helpfulman" },
	{ "heroinc1", "Hero, Inc Part One: Calling All Heroes" },
	{ "holygrail", "The Holy Grail" },
	{ "indarkness", "In Darkness" },
	{ "inheritance", "Inheritance" },
	{ "insanitycubed", "Insanity Cubed" },
	{ "islandinfinity", "The Island of Infinity" },
	{ "journeyking", "The Journey of the King" },
	{ "kaged", "Kaged" },
	{ "katanatads", "Katana" },
	{ "kissingbuddha", "Kissing the Buddha's Feet" },
	{ "kitchenencounters", "Kitchen Encounters" },
	{ "letheflowphoenix", "Lethe Flow Phoenix: A Flight of Fantasy" },
	{ "losingyourgrip", "Losing Your Grip, a Journey in Five Fits" },
	{ "lost", "Lost" },
	{ "lostnytads", "Lost in New York" },
	{ "lovesparade", "Everybody Loves a Parade" },
	{ "maidenmoonlight", "Maiden of the Moonlight" },
	{ "meanstreetstads", "Mean Streets" },
	{ "meine", "Meine Dalix" },
	{ "modernism", "Modernism" },
	{ "moist", "Moist" },
	{ "mrsk", "Magic Realms: The Sword of Kasza" },
	{ "murmellius2", "Murmellius 2" },
	{ "museumforever", "A Night at the Museum Forever" },
	{ "museumoftads", "Museum of TADS" },
	{ "mythtads", "Myth" },
	{ "nbdemo", "The Singular, and Historically Inaccurate, Misadventures of Nobeard" },
	{ "ninjaburger", "Ninja Burger" },
	{ "ntts", "No Time To Squeal" },
	{ "onefootdown", "One Foot Down" },
	{ "onegotaway", "The One That Got Away" },
	{ "pasttense", "Past Tense" },
	{ "pcu", "PC University: An Everyday Nightmare" },
	{ "perditionsflame", "Perdition's Flames" },
	{ "pesach", "The Pesach Adventure" },
	{ "polyadv", "PolyAdv - Colossal Cave" },
	{ "ptbad4", "PTBAD4: How the Record Got its Groove Back" },
	{ "ptbad5", "PTBAD5: Jesus Attack of Jesus Christ" },
	{ "rematch", "Rematch" },
	{ "reeferisland", "Reefer Island" },
	{ "sabotagecauldron", "Sabotage on the Century Cauldron" },
	{ "savannah", "Sunset Over Savannah" },
	{ "saveprincetontads", "Save Princeton" },
	{ "scavnightsedge", "Scavenger: Night's Edge" },
	{ "sda", "Special Detective Agent" },
	{ "seaofnight", "The Sea of Night" },
	{ "secondpit", "The Second Pit" },
	{ "sexartist", "The Sex Artist" },
	{ "shadowland", "Shadowland I: The Tower of Iron" },
	{ "shelby", "The Light: Shelby's Addendum" },
	{ "silenceofthelambs", "Silence of the Lambs" },
	{ "slapthatfish", "Slap That Fish" },
	{ "smallworld", "Small World" },
	{ "son0", "Prelude to Night" },
	{ "spyderandjeb", "Spyder and Jeb" },
	{ "stranded", "Stranded" },
	{ "suprematismblack", "Suprematism - Black" },
	{ "suprematismwhite", "Suprematism - White" },
	{ "tadsdungeon", "Dungeon" },
	{ "textmetroid", "Metroid" },
	{ "theapprentice", "The Apprentice" },
	{ "thecruise", "The Cruise" },
	{ "thegate", "The Gate" },
	{ "thehouset2", "The House" },
	{ "thelegendlives", "The Legend Lives!" },
	{ "themansion", "The Mansion or The Tale of the Adventurous Thief" },
	{ "themission", "The Mission" },
	{ "theoracle", "The Oracle" },
	{ "theplant", "The Plant" },
	{ "thewaterbird", "The Water Bird" },
	{ "tide", "The Ebb and Flow of the Tide" },
	{ "tilldeathmakes", "Till Death Makes a Monk-Fish Out of Me" },
	{ "tnc", "Tomorrow Never Comes" },
	{ "toonesia", "Toonesia" },
	{ "tortoise", "The Lesson of the Tortoise" },
	{ "tylerscube", "Tyler's Great Cube Game" },
	{ "underoos", "The Underoos that ate New York!" },
	{ "undertow", "Undertow" },
	{ "undo", "Undo" },
	{ "unease", "Unease" },
	{ "urbancleanup", "Urban Cleanup" },
	{ "uux", "Unnkulia X: Escape of the Sacrificed" },
	{ "veritas", "Veritas" },
	{ "videobar", "I'm Gonna Take You To The Video Bar!" },
	{ "waystation", "Waystation " },
	{ "worldsapart", "Worlds Apart" },
	{ "wormhole", "Wormhole: The Beginning" },
	{ "xarbo", "Escape from the Arboretum" },
	{ "yourchoice", "Your Choice" },
	{ "zebulon", "Uncle Zebulon's Will" },
	{ "zerosum", "Zero Sum Game" },

	// Adventions games
	{ "ccr", "Colossal Cave Revisited" },
	{ "legendlives", "The Legend Lives!" },
	{ "rylvania", "The Horror of Rylvania" },
	{ "u0", "Unnkulia Zero: The Search for Amanda" },
	{ "uhalf", "Unnkulia One-Half: The Salesman Triumphant" },
	{ "uu1", "Unnkulian Underworld: The Unknown Unventure" },
	{ "uu2", "Unnkulian Unventure II: The Secret of Acme" },

	// Commercial games
	{ "onceandfuture", "Once and Future" },

	{ nullptr, nullptr }
};

const PlainGameDescriptor TADS3_GAME_LIST[] = {
	{ "tads3", "TADS 3 Game" },

	// English games
	{ "allhope", "All Hope Abandon" },
	{ "blightedisle", "Blighted Isle" },
	{ "captivity", "Captivity" },
	{ "combatdemo", "TADS 3 Combat Demos" },
	{ "comp02tr", "Comp02ter Game" },
	{ "cookoff", "Cook Off" },
	{ "darkangel", "Dark Angel" },
	{ "deadmansparty", "Dead Mans Party" },
	{ "dearbrian", "Dear Brian" },
	{ "deezlebub", "Deezlebub" },
	{ "devildetails", "The Devil in the Details" },
	{ "elysiumenigma", "The Elysium Enigma" },
	{ "foreveralways", "Elizabeth Hawk's Forever Always" },
	{ "golden-banana", "The Quest of the Golden Banana" },
	{ "gunmute", "Gun Mute" },
	{ "homeopen", "Home Open" },
	{ "indigo", "Indigo" },
	{ "it", "It" },
	{ "edgeofchaos", "Jay Schilling's Edge of Chaos" },
	{ "korenvliet", "Korenvliet" },
	{ "lookaroundthecorner", "Look Around the Corner" },
	{ "lydiasheart", "Lydia's Heart" },
	{ "macrocosm", "Macrocosm" },
	{ "marthasbigdate", "Martha's Big Date" },
	{ "mrspepper", "Mrs. Pepper's Nasty Secret" },
	{ "multiverserogue", "Rogue of the Multiverse" },
	{ "mylastrodeo", "My Last Rodeo" },
	{ "pekostory", "Peko's Story" },
	{ "ratincontrol", "Rat In Control" },
	{ "recluse", "Recluse " },
	{ "returntoditchday", "Return to Ditch Day" },
	{ "ribosoperation", "Dr Who and the Ribos Operation" },
	{ "saturnschild", "Saturn's Child" },
	{ "sawesome", "Hide and Seek - Cindy's Something Awesome Project" },
	{ "spaceshooter", "Space Shooter: A TADS 3 abuse" },
	{ "squarecircle", "Square Circle" },
	{ "stormshelter", "Shelter from the Storm" },
	{ "thehouset3", "The House" },
	{ "thereveal", "The Reveal" },
	{ "threedaysofnight", "Three Days of Night" },
	{ "thesleepingprincess", "The Sleeping Princess" },
	{ "undertaleepilogue", "Undertale: Epilogue" },
	{ "venice", "A Lady in Waiting" },
	{ "vividity", "Vividity" },
	{ "wardz", "Ward Z" },
	{ "walkersilhouette", "Walker and Silhouette" },
	{ "wilderness", "In the Wilderness" },
	{ "wolfenschtein", "Wolfenschtein" },
	{ "writingonthewall", "The Writing on the Wall" },
	{ "wutheringheights", "Wuthering Heights" },
	{ "youmatched", "It's a Match!" },

	// Czech games
	{ "exoter", "Exoter" },

	// German games
	{ "pionierin", "Die Pionierin und die Tragödie des telematischen Typewriters" },

	// Italian games
	{ "remerook", "La missione di Kyle Remerook" },

	// Spanish games
	{ "verla", "No Quiero Verla" },

	{ nullptr, nullptr }
};

const GlkDetectionEntry TADS_GAMES[] = {
	// TADS 2 - English
	DT_ENTRY0("1893", "f4a361e9af20b3c8617188e5d9a42134", 70848900),
	DT_ENTRY0("2044", "ec9e00db314709ba9616ec1a7f9dd5ea", 48263),
	DT_ENTRY0("3", "db9086179d99638372803e5500c3154a", 107073),
	DT_ENTRY0("aayela", "b12057d8531dd36b899c70f339a01fe1", 110755),
	DT_ENTRY0("aboveandbeyond", "d3d84a29c0a8c2b2ab1007a42f38f7e4", 548383),
	DT_ENTRY0("alienabduction", "1ac6cb8ffd802663e77669c4493347d9", 192983),
	DT_ENTRY0("allalone", "2137e080d79510d0655106e6d9f0d3ae", 154087),
	DT_ENTRY0("amissville", "e6ffe067bdb05576e4ac1f4ea96075fa", 1021385),
	DT_ENTRY0("amoi", "f47a0612771c0e84589ffde45aa864f7", 469943),
	DT_ENTRY0("anightguest", "b4d64bfece91b350475c93b27e58efd5", 261550),
	DT_ENTRY0("arrival", "41b1af240774f3ce8217d4acb5deb2dc", 404608),
	DT_ENTRY0("arrival", "41b1af240774f3ce8217d4acb5deb2dc", 1313912),
	DT_ENTRY0("asimpletheft", "4e5432f0bed40ec0b04aab92bda10cd6", 74599),
	DT_ENTRY0("asugaredpill", "c1cc5d1d481d7dda8184f12c276e2de8", 654151),
	DT_ENTRY0("awe", "538e51543e14ea8fd69847ae070af1ab", 448382),
	DT_ENTRY0("babel", "d0eb2c258829f97a988d168fad6aa891", 465717),
	DT_ENTRY0("badmachine", "274c4c2bf0008134b924851d19511ab0", 331577),
	DT_ENTRY0("ballymunadv", "9ec0690e89491e1407b3170ad31d814a", 344557),
	DT_ENTRY0("beetmonger", "e24f782fba8db13fac45669b7fb31945", 463248),
	DT_ENTRY0("bmiss", "35ca3147c38a022a82048e1aa61562a8", 121787),
	DT_ENTRY0("brokenstring", "bb49b4d32b1cd760501ef843ebe43f16", 194718),
	DT_ENTRY0("castleofdoom", "5a09aa5ebb8a62f94d9f33e081ef0ef4", 51848),
	DT_ENTRY0("coffeequest", "9caa2da66fe0d510adc94c775edc00c5", 190195),
	DT_ENTRY0("commonground", "1e074e89c7a5aeb917661d3fd21d276c", 262320),
	DT_ENTRY0("ceruleanstowaway", "46e9b2205a5cf092b22ff68fae254c0f", 244366),
	DT_ENTRY0("coasthouse", "ba255c85d9565aef0c84e209cbcc7115", 151718),
	DT_ENTRY0("dampcamp", "17d330ba999713b65ebafb1527bbc84b", 87845),
	DT_ENTRY0("deanna", "34570038fd9661c0c5235e52517644d5", 236165),
	DT_ENTRY0("deepspace", "636e58c425e066335c6f92e7bfb0d5f7", 203942),
	DT_ENTRY0("depravitybites", "b918fef79d70a12b048d413689d03495", 170029),
	DT_ENTRY0("diemasse", "f1c42af81468d8d69f2726e3b56e8449", 211069),
	DT_ENTRY0("ditchday", "c8f23c03638639c31f7e9602d09fe9ea", 127622),
	DT_ENTRY0("drool", "d2b78f16da2a2f8c51cbbef0e00001a1", 291119),
	DT_ENTRY0("drool", "d2b78f16da2a2f8c51cbbef0e00001a1", 1955771),
	DT_ENTRY0("e14s", "23c3aa566d41d325c1aa474ff6a9360a", 163826),
	DT_ENTRY0("ecdysis", "ea91694e025c748af40079c4bbcbf995", 114423),
	DT_ENTRY0("ecdysis", "ea91694e025c748af40079c4bbcbf995", 141344),
	DT_ENTRY0("edgarhuntly", "fb61663dda2a6d520cfc6de106075ca5", 158401),
	DT_ENTRY0("enhanced", "fe4e227aaac4f4ab27a77944851603cc", 186737),
	DT_ENTRY0("escapecrazyplace", "8540e96ba6475ef8232295b5b82c3ab8", 2299708),
	DT_ENTRY0("eterna", "5d837d71e2b19236f9bb8fdb8f57169c", 146228),
	DT_ENTRY0("excuseme", "897cfc0406e955f3b87ce95ca4d6df15", 332648),
	DT_ENTRY0("exhibition", "53e06a49a8902bb5b7502bbeb5fcce08", 4380469),
	DT_ENTRY0("findingmartin", "1c92590cd85cb707e4e2763a825515de", 1547636),
	DT_ENTRY0("firebird", "487f019e24cdfbeef733e11d60b18c08", 349270),
	DT_ENTRY0("firewitch", "cd40d433dfb513d45d9dae58a098d3cd", 154517),
	DT_ENTRY0("fms", "b15de56d4c675b4397bb73a71e66a6e9", 165365),
	DT_ENTRY0("foom", "d951126d5e59ee117e35240c24856278", 106100),
	DT_ENTRY1("foom", "Debug", "37f19fd1d15ff19c35ffafde36513ff0", 169116),
	DT_ENTRY0("fourin1", "3ff69b30fdce7042e8d01303302161f3", 304429),
	DT_ENTRY0("fren51", "584ed64193d4ccfaf4b5bf70f5aa8c4f", 250729),
	DT_ENTRY0("fren52", "dfffed6aee17d45b79a6046376add419", 201302),
	DT_ENTRY0("fren53", "6069167beb127d0a281932f9b047ddc9", 380739),
	DT_ENTRY0("friendlyfoe", "194719a7f6453e4a94636cce6819aea4", 238026),
	DT_ENTRY0("frustration", "2c7f953e8448d9c0b5989a8233099fb7", 258786),
	DT_ENTRY0("ftf", "b80c3f3309d2c0ae2ef44d0838a9881b", 1220695),
	DT_ENTRY0("fusillade", "a2489852bcf124e2c65a0588c7c8e86f", 268065),
	DT_ENTRY0("fusillade", "a2489852bcf124e2c65a0588c7c8e86f", 356756),
	DT_ENTRY0("galacticpeace", "b7d4c41b0a2225207c3fd39c1bb608a6", 111044),
	DT_ENTRY0("gc", "cfc900f65aaab118ad3621344ea0f45e", 305128),
	DT_ENTRY0("gift3", "3da61fbf2ae0309cfe68633bad57a7aa", 32864239),
	DT_ENTRY0("gna", "a556ddb456c3f230b4ac9554a1bcae13", 922981),
	DT_ENTRY0("gna", "a556ddb456c3f230b4ac9554a1bcae13", 1968152),
	DT_ENTRY0("goldenfleece", "0297cc60d93409c33458cc17e26d97cd", 199983),
	DT_ENTRY1("goldenskull", "Demo", "551fb6d8bac8c169343a33a9211bc055", 1173687),
	DT_ENTRY0("hamper", "9159eb24bc9311f4ca51916ca8200e9d", 251739),
	DT_ENTRY0("helpfulman", "2b73d801672bb49ba75e2eac680ee469", 351592),
	DT_ENTRY0("helpfulman", "2b73d801672bb49ba75e2eac680ee469", 979505),
	DT_ENTRY0("heroinc1", "1d80dd1ffda1270926deb7a1745f9ccf", 148427),
	DT_ENTRY0("holygrail", "a837f2c5a2ba17361c52f4e567211d7a", 216242),
	DT_ENTRY0("indarkness", "365696166db6e16bc4dfaec94ab73534", 489743),
	DT_ENTRY0("inheritance", "18e17f76b038b95e1ad99d47a1c5ef55", 106396),
	DT_ENTRY0("insanitycubed", "5ed65e5000a2c394eec872ab71ed2ae7", 117209),
	DT_ENTRY0("islandinfinity", "c60957d69aaf2b2438fa6ab9185217e5", 248815),
	DT_ENTRY0("journeyking", "f68126f75c8134d26ea348b560f7cae2", 310554),
	DT_ENTRY1("kaged", "No Sound", "94cfb4696ecb029b4a3ad354d0e5e842", 1031879),
	DT_ENTRY0("katanatads", "bcf79886ddd9869f1e8a19b5cf186075", 828033),
	DT_ENTRY0("kissingbuddha", "715e9bd3042346f2a179e903ea83763b", 207755),
	DT_ENTRY0("kitchenencounters", "a73484097e4e24c2226a8361a2dc0009", 70272),
	DT_ENTRY0("letheflowphoenix", "e3760cc303082aaa06b54e0b4a6358e8", 272075),
	DT_ENTRY1("letheflowphoenix", "No I/O", "a5e60a545bcad6bbb4ebaf532ae7e745", 272140),
	DT_ENTRY0("losingyourgrip", "4c6373e6db29f6b1671cd61690722e53", 713150),
	DT_ENTRY0("lost", "a30d7caa49f58ed305fc669b4c5b9ecf", 297358),
	DT_ENTRY0("lostnytads", "93212a4f1feac069eb13d97f53c41444", 434674),
	DT_ENTRY0("lovesparade", "66f7f4cfe536e1a80c87c302b5fdc490", 270763),
	DT_ENTRY0("maidenmoonlight", "c3f7bb755c0945af26d0579ebd8a087f", 216202),
	DT_ENTRY0("meanstreetstads", "79a6207bbb92faac485e4c4677fa3ab3", 127027),
	DT_ENTRY0("meine", "bf118303cd6c599f281070f9177bf15f", 229180),
	DT_ENTRY0("modernism", "1a80f4b557d6c7dc94a906baab57edee", 46549),
	DT_ENTRY0("moist", "2cb2b56e00df7148b7c40c4753c801d5", 373100),
	DT_ENTRY0("mrsk", "c21cedc30ce3d6eadefc2f75f5b1a168", 224542),
	DT_ENTRY0("murmellius2", "97df7e025e1a9f1ba1523664ad4ead9c", 105661),
	DT_ENTRY0("museumforever", "a27ee66cedfcc2e0e83740ebe6e586f9", 124634),
	DT_ENTRY0("museumoftads", "829d1af57ff47169ad4d7e6551a22721", 253171),
	DT_ENTRY0("mythtads", "63b1eee145cd8983cb645328c9e78cdf", 161000),
	DT_ENTRY1("nbdemo", "Demo", "eac25992e40de6aee5e36bc3195f21a9", 112331),
	DT_ENTRY0("ninjaburger", "f4edb87ecf3e0e28ff0442a8b03e1dbb", 94376),
	DT_ENTRY0("ntts", "32b0b4ccc1d208e7d163790074e434ce", 601033),
	DT_ENTRY0("onceandfuture", "b2d13457625706ba4c4015d293f83a63", 918200),
	DT_ENTRY0("onefootdown", "f8e9be6afe3eb6b5296ebd098f638421", 210220),
	DT_ENTRY0("onegotaway", "df697076c90cb474a6ef5cf6fe15b06d", 108570),
	DT_ENTRY0("pasttense", "8422454168ee6bc2d45385ce629c5c22", 195897),
	DT_ENTRY0("pcu", "4e82b5774dab9948a0331a80075ed7c8", 183145),
	DT_ENTRY0("perditionsflame", "4b2307fb8ff4095de94c9dcc0b68504e", 334065),
	DT_ENTRY0("pesach", "205aa10d378fc0bd72fad6f4d565ac07", 100556),
	DT_ENTRY0("polyadv", "52430065c2953733074474674a65c411", 884370),
	DT_ENTRY0("ptbad4", "81bccf14bd80c405bed38b8935b212dd", 58290),
	DT_ENTRY0("ptbad5", "57e824ca2b61bcb7ad89d6410a66ec77", 59858),
	DT_ENTRY0("reeferisland", "e1c33e8e06fd335870c1cecec782e02b", 364587),
	DT_ENTRY0("rematch", "90d3b611eee24387da7c911637d05757", 244009),
	DT_ENTRY0("sabotagecauldron", "181c7bf3a7928443b185f8e7025b7c22", 324350),
	DT_ENTRY0("savannah", "b73eccfcfab113f27b3a477f894ae882", 547813),
	DT_ENTRY0("saveprincetontads", "2159c4ff547d493f90c0bec0dfa1dcf6", 273825),
	DT_ENTRY0("scavnightsedge", "17b62ca221ad5f046285594a57d6c0c0", 473137),
	DT_ENTRY0("sda", "6fb2b989c85226cc422c511ad404ce48", 622454),
	DT_ENTRY0("sda", "6fb2b989c85226cc422c511ad404ce48", 166208),
	DT_ENTRY0("seaofnight", "c3f6293fa675d74c5ab179f024354720", 95798),
	DT_ENTRY0("secondpit", "0aee7dca101b9f20e533ce32a8178535", 370175),
	DT_ENTRY0("sexartist", "8256e0b96819b15a2f083d6011c02573", 822794),
	DT_ENTRY0("shadowland", "d1a187f4b8acacb5fde1ab9510c5949c", 157551),
	DT_ENTRY0("shelby", "0d39d9271cced70cba53426d17d197b8", 398947),
	DT_ENTRY0("silenceofthelambs", "ca9483a874cf2cf126b6043b222f1ee1", 45157),
	DT_ENTRY0("slapthatfish", "aa8a6c502f1f1582654c0942ad2231be", 144235),
	DT_ENTRY0("smallworld", "74b95dde89a25a61d28d380ab4ea1004", 262067),
	DT_ENTRY0("son0", "cebd3ee5cf23fdc5737585c39de8fb3d", 87976),
	DT_ENTRY0("spyderandjeb", "4003cfc9734df7ab20377f9fe30365ba", 90655),
	DT_ENTRY0("stranded", "74a0aa9b64d910eab42ae038e8477b9c", 376112),
	DT_ENTRY0("suprematismblack", "c45e05bd86bc6ae6573810bfaff26a79", 54614),
	DT_ENTRY0("suprematismwhite", "caf1756fad48f2cc008986b20e29d82e", 56991),
	DT_ENTRY0("tadsdungeon", "5e78e4f4655c42b7bbf32e4088595eb2", 290310),
	DT_ENTRY0("textmetroid", "4399099ee07f186ec05972cfdd7b10ea", 56436),
	DT_ENTRY0("theapprentice", "dca00dd78cbf7ca1df6916547672f098", 178406),
	DT_ENTRY0("thecruise", "fd7cae80b84d0229f51f0f647ba674f7", 192995),
	DT_ENTRY0("thegate", "e88364be7f027da095eaab6a5150575a", 68294),
	DT_ENTRY0("thehouset2", "9b45d0ac097e99f6d47a105e52edda16", 83981),
	DT_ENTRY0("thelegendlives", "33d0c4bb222c0267eef1f750526bc57f", 623095),
	DT_ENTRY0("themansion", "e47b961ffc30828f15551ac738192362", 95515),
	DT_ENTRY0("themission", "6614dea95ebb3b54e2ee0d3c66a819da", 254145),
	DT_ENTRY0("theoracle", "4565348d87a51856558000a018812e25", 172090),
	DT_ENTRY0("theplant", "312894d51640eb9012ec5130059f0b58", 384092),
	DT_ENTRY0("thewaterbird", "49aa9dd604d21d6cc218ba72fb18a214", 280832),
	DT_ENTRY0("tide", "ae0026feaf23a2d9987e77901f107622", 89757),
	DT_ENTRY0("tilldeathmakes", "346c068e777e6bddfd85ad851bbe8809", 566548),
	DT_ENTRY0("tnc", "fe79042b10d727c04cd23fb54f185e0a", 898785),
	DT_ENTRY0("tnc", "fe79042b10d727c04cd23fb54f185e0a", 1034403),
	DT_ENTRY0("toonesia", "54f9a3efea8faf6acf99629fdf787ad2", 126627),
	DT_ENTRY0("tortoise", "a6a2d324fabf5594c5950c17c52c7d44", 91936),
	DT_ENTRY0("tylerscube", "9be988b677ed05ff12ccc66ab3eda535", 39002),
	DT_ENTRY0("underoos", "877f6a899c08b312afbbe31158c0689a", 68526),
	DT_ENTRY0("undertow", "1a178cde22a95108f5f176c58afa07df", 159487),
	DT_ENTRY0("undo", "d8be101bce162538bc1713530936e720", 58707),
	DT_ENTRY0("unease", "7c1709cb482498e00b9e70da7a33e605", 412858),
	DT_ENTRY0("urbancleanup", "b632f5f31e70ebfaf2ad2bd09fda47b0", 149135),
	DT_ENTRY0("uux", "20b3b72ec1143f8ddb91e19dd209e9a0", 885548),
	DT_ENTRY0("veritas", "8ae2688ffa2cd25c4eac70c16688105b", 322359),
	DT_ENTRY0("videobar", "1f335e8faeddb2736d3d17517a737f25", 1553252),
	DT_ENTRY0("waystation", "2e9171818b2fc9bbb7c0ffbe8d97bcbb", 290376),
	DT_ENTRY0("worldsapart", "52b46186628e9a86d860aba91539287c", 1182959),
	DT_ENTRY0("wormhole", "3e15f6f2c84eb3a0837f433aa8d0d8a1", 173579),
	DT_ENTRY0("xarbo", "24c991db36a721192b5f974f55de1719", 285594),
	DT_ENTRY0("yourchoice", "72f5727f770c6d355b6e5de32f05109e", 83855),
	DT_ENTRY0("zebulon", "6be895d5655514b5a5b5c97b9b245120", 147473),
	DT_ENTRY0("zerosum", "286ede6ac6deaa588d98252ad9207013", 296769),

	// TADS 2 - Commercial games
	DT_ENTRY0("onceandfuture", "4ed995d0784520ca6f0ec5391d92f4d8", 909993),

	// TADS 2 - Adventions games
	DT_ENTRY0("ccr", "2da7dba524075aed0167ae02e6484a32", 291305),
	DT_ENTRY0("legendlives", "03a651ef3d904dbef6a80cd2c041423c", 622989),
	DT_ENTRY0("rylvania", "98027d23f0da0e38c02f1326a2357713", 302903),
	DT_ENTRY1("u0", "Demo", "d9615e08336cc0d50984cdc0879f2006", 500949),
	DT_ENTRY0("uhalf", "80c18f27f656d7e83d1f3f8075115720", 242269),
	DT_ENTRY0("uu1", "705b91f9e54c591e361d6c2e15bff8ef", 367506),
	DT_ENTRY0("uu2", "69a4f7cd914bd32bd7a3bef5c68f9f3a", 400113),

	// TADS 2 - Russian
	DT_ENTRYL0("ditchday", Common::RU_RUS, "30743db95826c38a4e683f44dc5fe2d2", 206234),

	// TADS 3 - English
	DT_ENTRY0("allhope", "1e842218ad6eeacb2d284ad34adbeaaa", 1816249),
	DT_ENTRY1("allhope", "WebUI", "c65cff5c63cfecad3d8c922f0086816f", 2434146),
	DT_ENTRY0("blightedisle", "a400f71d73c0efdf301635e73ff00c79", 3326591),
	DT_ENTRY1("blightedisle", "WebUI", "4ee80cdf2e39f41e721440db7ddbc05e", 3999970),
	DT_ENTRY0("captivity", "aeeb0bcd430448867d45a4e6bea96c23", 1908803),
	DT_ENTRY0("combatdemo", "23a59262f9d8534562379b123e1272b7", 515392),
	DT_ENTRY0("combatdemo", "23a59262f9d8534562379b123e1272b7", 620151),
	DT_ENTRY0("comp02tr", "23327e39fa44259fcabcb99917302c22", 960653),
	DT_ENTRY0("cookoff", "581d1996db3a442064dae237e2e962f7", 1147531),
	DT_ENTRY0("darkangel", "04f418d9e6ad8fb5a8d8291e11a788e6", 2873281),
	DT_ENTRY1("darkangel", "WebUI", "f7b2b242d4029623171f35214ef213c8", 2873281),
	DT_ENTRY0("deadmansparty", "bc78f6e390df143a13aeabcdb9de1fed", 1638558),
	DT_ENTRY1("deadmansparty", "WebUI", "831ba48e61a40e442a7459aa798da3ea", 2056080),
	DT_ENTRY0("dearbrian", "8488c4f5c18a76ab65dd9e8eb72393ac", 999423),
	DT_ENTRY0("deezlebub", "57d532aa89fc21abaa52745689326a5f", 1142633),
	DT_ENTRY0("devildetails", "0cca588557a79231daa78ef948f62397", 2892947),
	DT_ENTRY0("elysiumenigma", "f98f0e4de8c7cf5d6f471b56c6a96d81", 2095654),
	DT_ENTRY1("elysiumenigma", "WebUI", "7ca73a03b433b28a831808066a843d79", 2149533),
	DT_ENTRY0("foreveralways", "1e901bfebf416a910ba55e301101034c", 656194),
	DT_ENTRY0("golden-banana", "c5df512d642503556cd9c0d487a3c95f", 726650),
	DT_ENTRY0("gunmute", "bf196b6b53fca37c8fec25d27bdb4f23", 713636),
	DT_ENTRY0("homeopen", "ae46177333238267362f9465f65fac80", 1869401),
	DT_ENTRY1("homeopen", "WebUI", "7db13d7785329886ad5f24533115a749", 2303325),
	DT_ENTRY0("indigo", "ca322b887ba56ef503b96ee462a2df30", 535570),
	DT_ENTRY0("it", "aae7a1a7eb812ef216406875df1035bc", 1161138),
	DT_ENTRY1("it", "WebUI", "e69e00d227df2ea9179bb3bc20d779c9", 1589779),
	DT_ENTRY0("korenvliet", "4ab26eb23354a000f66b77bcbb820be4", 719957),
	DT_ENTRY0("korenvliet", "9c91716c269e268938c5051dbfc07f33", 1311919),
	DT_ENTRY0("edgeofchaos", "6e20b8013ff2e8cc1a9e1b0ce570adbb", 11780522),
	DT_ENTRY0("lookaroundthecorner", "c9ac24c941c39ef57b23a55ed36f844d", 664271),
	DT_ENTRY1("lookaroundthecorner", "WebUI", "5e7d1e0c7f93662fb455972797795f15", 1079205),
	DT_ENTRY0("lydiasheart", "aa92fa47c4e04e90d358e9cfd50de671", 2620069),
	DT_ENTRY0("macrocosm", "44492ef14565d51cbfe8ae6ddf07af1f", 30678531),
	DT_ENTRY0("marthasbigdate", "8b65ae35ad2165d8bdc48f33e7b2bbb2", 646813),
	DT_ENTRY0("mrspepper", "d9b3c2f7b954fb066561febf195b27d4", 1118901),
	DT_ENTRY1("mrspepper", "WebUI", "9bcfcdc5a37e9c5beb9b961a98929b67", 1721042),
	DT_ENTRY0("multiverserogue", "551b85aaba2d958f2bfd1080c7787844", 811469),
	DT_ENTRY0("mylastrodeo", "82f4a03bfdd5b8da41c6e3dbba0d9666", 1614970),
	DT_ENTRY1("mylastrodeo", "WebUI", "d7f480254686e114037de11f9c82782b", 2042237),
	DT_ENTRY0("pekostory", "0d703060d485aa4aeefe0d267a819ab3", 1100463),
	DT_ENTRY0("ratincontrol", "607502409e7143cb20896d09201744ac", 401192),
	DT_ENTRY0("recluse", "c41b5ab77e458f38c487dc1a1e0778f2", 1073030),
	DT_ENTRY0("returntoditchday", "47577d2001035ac30332bdb5ad9bcf3c", 2141661),
	DT_ENTRY0("ribosoperation", "7ce5c5c74b5adecc8520a99e720a6a0d", 718057),
	DT_ENTRY0("ribosoperation", "19b7227f381f4e9491f3e2b7d9bccb59", 714946),
	DT_ENTRY0("saturnschild", "2644265c684d153777dc8477e9e51ce9", 2321035),
	DT_ENTRY1("saturnschild", "WebUI", "67b0926a6b96a37a88e4564e3481f386", 2795440),
	DT_ENTRY0("sawesome", "558a930adf7995b566d901719cb90041", 1227313),
	DT_ENTRY0("spaceshooter", "cb7275fb06421696153aef808bc28d53", 79989),
	DT_ENTRY0("squarecircle", "935775a75e141df9a6f9d94b41ae71ee", 1024972),
	DT_ENTRY0("stormshelter", "72d6180f16066cdb20725176d03d7ded", 1702075),
	DT_ENTRY1("stormshelter", "WebUI", "db84c1f25835763494fc8124fcade37f", 2293053),
	DT_ENTRY0("thehouset3", "8b92f7f00ef0ca5713f7ffffa13b8a01", 1688640),
	DT_ENTRY0("thereveal", "ff5485af8cbc5c0b4211bf87db23ceeb", 1590214),
	DT_ENTRY1("thereveal", "WebUI", "d4f196d2e6bd3dd49f70e52bb504e793", 2018665),
	DT_ENTRY0("thesleepingprincess", "63255d1bde649f907725a6c328047809", 736075),
	DT_ENTRY1("thesleepingprincess", "WebUI", "711cf6098b0670821af907dc94793e90", 1337974),
	DT_ENTRY0("threedaysofnight", "8271c716c6c11b4aec8b0b95aacb276d", 1711576),
	DT_ENTRY1("threedaysofnight", "WebUI", "1de414e58b667eab103361dbcd95353a", 2139562),
	DT_ENTRY0("undertaleepilogue", "e76c15c3446865916bb1647ed4f83b3e", 1189729),
	DT_ENTRY0("venice", "2db783be567e7761b4501ec12bc2eb35", 880111),
	DT_ENTRY1("venice", "WebUI", "e0e9efc4c6a680ac60ddc380c13cc099", 1306352),
	DT_ENTRY0("vividity", "7da90c0a140c73249b83b5c3e459ca83", 692049),
	DT_ENTRY1("vividity", "WebUI", "f535eb35d8f4776b2262dd8aca3281b3", 1106715),
	DT_ENTRY0("walkersilhouette", "05bd96f0782fefa3d09d0adad67249c9", 687377),
	DT_ENTRY0("wardz", "f17c720b4a80693534fea474a54f15b4", 1170346),
	DT_ENTRY0("wilderness", "5ea2cc9ab1193d416c907b9c1f0934e3", 663353),
	DT_ENTRY1("wilderness", "WebUI", "b2a76f20abf3f255dfe23d53cbaedcf0", 1089562),
	DT_ENTRY1("wolfenschtein", "WebUI", "31ccf0d040271950ddf3ca91f8ff22e2", 1200645),
	DT_ENTRY0("writingonthewall", "85555df67c45ed04cfd18effb7bbd670", 1138559),
	DT_ENTRY0("wutheringheights", "0d2042689bdf63e719b77485d591f1fe", 1083215),
	DT_ENTRY0("youmatched", "63007bd45363f5ddbcc2e4fe4a1a52de", 1621574),
	DT_ENTRY1("youmatched", "WebUI", "055efcc37f945071ea2486a207703951", 2050047),

	// TADS 3 - CZech
	DT_ENTRYL0("exoter", Common::CZ_CZE, "02b93382a19cd69f3cb67a12073f4795", 1487144),
	DT_ENTRYL0("exoter", Common::CZ_CZE, "865db2452b38f96035841f04e314c2c6", 1489600),
	DT_ENTRYL0("exoter", Common::CZ_CZE, "956f93c8c8b5270d75501039ef825429", 1074752),
		
	// TADS 3 - German
	DT_ENTRYL0("pionierin", Common::DE_DEU, "9f899c9826204184c09f7088acfa8cce", 1293016),
	DT_ENTRYL0("pionierin", Common::DE_DEU, "11b10b38003dda7e7c0c43bac3ce67d0", 928810),

	// TADS 3 - Italian
	DT_ENTRYL0("remerook", Common::IT_ITA, "218f1ad01f58f93a610ba91f626b5fb5", 1166951),

	// TADS 3 - Spanish
	DT_ENTRYL0("verla", Common::ES_ESP, "751eaee6249e4acec9b7ace03bb80fb4", 1118875),

	DT_END_MARKER
};

} // End of namespace TADS
} // End of namespace Glk
