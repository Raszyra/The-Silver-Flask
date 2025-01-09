#include "resources\\scripts\\tsf_resources.h"

using namespace std;

/*Title information

<================================>
|>>>>>>[ THE SILVER FLASK ]<<<<<<|
<================================>
(c) 2023 Will Hooker
Current version: 0.5.2 (alpha)

A console game that began as a systems test.

Grew to be a full-fledged RPG, containing
-boss fights
-side quests
-a varied 100-square map
-200+ different items
-25 different character classes
-3 class progression trees
-40 different enemies
- random encounters
-companion characters
-recurring mini-quests
-survival and crafting skills
-60+ unique spells and abilities
- more


(project began Sep. 2020.)

|
|
+--------------------------------+

*/


/* -->Notes:
<=======================>
|
|

ADD :" Winmm " for sound effects

Note: smithing shortswords, longswords, small shields, kiteshields and daggers
      yields one extra point of attack and defense over the vanilla versions.
|
|
+=======================>
*/


/* -->Current Projects
<>----------------------------------<>
 |  ####> CURRENT PROJECTS <####    |
<>----------------------------------<>
|                                   |
Version: 0.5.2                    |
|                                   |
|
        <(fix)> BUG: successful enchantments crash the game
        <(fix)> BUG: some skills crash the game when used in combat
        <(fix)> BUG: game crashes on entering Lava Plains
        <(___)> BUG: Smithing a steel bar returns an empty item (without consuming coal)
        <(___)> BUG: Loading a new save crashes when populating elist[]
                        - Bug triggered when loading elist[2] 'Viper'

    [ ] Smithing overhaul
        x Ore selection
        x Item copy
        - missing items
            - wakizashi
            - Steel bar

    [1/4] Music
        - Add notes for music playing
        - fix aria's quest notification
        x Weather song bugs ( tested with Bard() )
        - save music xp

    [ ]Search and destroy:
        -typos
        -formatting errors

    [2/4] Companion character NPCs
        - Frost
        x Grognak
        - Aldo
        x Luz

    [4/7] Complete save/load
        x Quest saving bug
        x Trophies
        - Custom commands
        x Kill counters
        - NPC deaths
        - Player-built buildings
        x Season, time of day, year
        - Skills (academic)

 --> base game progress:

   [2/10]main quests

   [23/25]side quests

   [8/8]guild quests

   [34/41]NPCs

|
|                                     |
|                                     |
<>-----------------------------------<>

*/

//global declarations
string times;

//player
int pctox=0;
bool admin;

//"other"
int bank;
string notes;

//
string basic_command_list=" 'menu' to access the main menu\n 'help' to access commands, tutorials, and game tips\n 'save' to save your game, 'load' to open a saved game\n 'area' to look around,\n 'move' to move north, south, east or west,\n 'chest' to open any treasure chests you find,\n 'take' to pick up an item,\n 'fight' to  battle a nearby enemy,\n 'hpot' or 'mpot' to drink a health or mana potion,\n 'inv' to see items, potions and gold,\n 'tools' to see a list of tools/crafting equipment,\n 'stats' to view player name, level, atk, etc.,\n 'equip' to use an item from inventory,\n 'unequip' to store a held item\n 'drop' to drop an item on the ground\n 'char' to see a description of your character, or\n 'customize' if you don't have one yet\n 'skills' to see your skill levels\n 'spells' to see unlocked abilities\n 'map' to view the game world\n 'look' to inspect people, places or items\n";
//

void show_color_map()
{
    for (const auto& pair : console_colors) {
        if ( pair.first != "black" ) SetConsoleTextAttribute(hConsole, pair.second);
            else SetConsoleTextAttribute(hConsole, 7);
        cout << " - " << pair.first << "\n"; // Display color names
    }
    SetConsoleTextAttribute(hConsole, game_settings.mtcol);
}

int get_color_code( const string color_name, const vector<pair<string, int>>& console_colors )
{
    auto it = find_if(console_colors.begin(), console_colors.end(), [&color_name](const pair<string, int>& pair) {
            return (pair.first) == (color_name);
    });

    if (it != console_colors.end()) {
        return it->second; // Return the color code
    } else {
        return -1; // Invalid color
    }
}

void choose_text_colors()
{
    show_color_map();

    prln("Enter the name of the background text color: ");
    string backg = pc.getInps();
    prln("Enter the name of the foreground text color: ");
    string foreg = pc.getInps();

    int n_back = get_color_code(backg, console_colors);
    int n_fore = get_color_code(foreg, console_colors);

    if (n_fore == -1 || n_back == -1) {
        cerr << "One or more invalid color(s) entered.\n";
        return; // Exit with error
    }

    else {
        //This sets the binary sequence relating to the color; <<4 | n_fore adds the foreground color (four characters) to the end.
        game_settings.mtcol = (n_back << 4) | n_fore;
        SetConsoleTextAttribute(hConsole, game_settings.mtcol);
        cout << "\nConsole color set. You have chosen "; colSet(n_fore, foreg); cout << " on "; colSet(n_back, backg); cout << ".\n";
    }
}

void choose_player_colors()
{
    show_color_map();

    int pblock[2] = {0, 0};
    string psym[2] = {"n", "n"};
    for (int i = 1; i < 3; i++) {
        cout << "\nChanging player map block: " << i << " of 2\n";
        prln("Enter the name of the background color: ");
        string backg = pc.getInps();
        prln("Enter the name of the foreground color: ");
        string foreg = pc.getInps();

        cout << "Enter character number " << i << " (or enter an underscore for a solid color block) :\n";
        psym[i-1] = pc.getInps();

        int n_back = get_color_code(backg, console_colors);
        int n_fore = get_color_code(foreg, console_colors);

        pblock[i-1] = (n_back << 4) | n_fore;
    }

    game_settings.pcol_1 = pblock[0];
    game_settings.pcol_2 = pblock[1];
    game_settings.psym_1 = psym[0];
    game_settings.psym_2 = psym[1];

    cout << "\nYour new map identifier is: ";
    colSet(game_settings.pcol_1, game_settings.psym_1);
    colSet(game_settings.pcol_2, game_settings.psym_2);
}

void settings()
{
    menu m_settings = {
        "\nSettings\n\n",
        "Change which setting?\n",
        5};
    m_settings.options.push_back("");
    m_settings.options.push_back("Toggle sound");
    m_settings.options.push_back("Display Settings");
    m_settings.options.push_back("Combat Wait Time [" + to_string(game_settings.combat_wait_time) + " sec]");
    m_settings.options.push_back("Difficulty Settings");

    do//int colors[16]={15, 17, 34, 51, 68, 85, 102, 119, 136, 153, 170, 187, 204, 221, 238, 255};
    {
        m_settings.display();
        pc.inp = pc.getInps();
        if(pc.inp=="1"){switch(game_settings.sound){case 0: {game_settings.sound=1; cout<<"Sound effects on.\n";} break; case 1: {game_settings.sound=0; cout<<"Sound effects off.\n";} break;}}
        else if(pc.inp=="2")
        {
            menu m_graphic_settings = {
            "\nDisplay Settings\n\n",
            "Enter a number to choose an option:\n",
            6}; //Number of options
            m_graphic_settings.options.push_back("");
            m_graphic_settings.options.push_back("Change main text color");
            m_graphic_settings.options.push_back("Change player icon colors");
            m_graphic_settings.options.push_back("Travel display [currently: " + game_settings.s_dsp_type + "]");
            m_graphic_settings.options.push_back("Status display [currently: " + game_settings.status_display + "]");
            m_graphic_settings.options.push_back("Damage display [currently: " + game_settings.combat_display + "]");
            m_graphic_settings.display();
            int n_choice = pc.getInpn();

            switch (n_choice)
            {
                case 1: { choose_text_colors(); } break;
                case 2: { choose_player_colors(); } break;
                case 3: //Travel
                {
                    switch(game_settings.display_type)
                    {
                        case 0: {game_settings.display_type=1; game_settings.s_dsp_type = "graphic"; cout<<"\nSwitched to ASCII graphics for travel and area display.\n";} break;
                        case 1: {game_settings.display_type=0; game_settings.s_dsp_type = "text"; cout<<"\nSwitched to text descriptions for travel and area display.\n";} break;
                    }
                } break;
                case 4: //Status
                {
                    if ( game_settings.status_display == "text") {game_settings.status_display = "graphic";
                        prln("Health, mana and experience will now be displayed with\nASCII graphic bars instead of descriptive text.");}
                    else if ( game_settings.status_display == "graphic") {game_settings.status_display = "text";
                        prln("Health, mana and experience will now be displayed with\ndescriptive text instead of ASCII graphic bars.");}
                } break;
                case 5: //Damage
                {
                    if ( game_settings.combat_display == "text") {game_settings.combat_display = "numeric";
                        prln("Damage dealt in combat will now be displayed with\nnumbers instead of descriptive text.");}
                    else if ( game_settings.combat_display == "numeric") {game_settings.combat_display = "text";
                        prln("Damage dealt in combat will now be displayed with\ndescriptive text instead of numbers.");}
                } break;
                case 6: {} break;
                default: prln("Input error! Menu option not recognized."); break;
            }
        }
        else if(pc.inp == "3")
        {
            prln("Enter a number of seconds to wait between turns:");
            game_settings.combat_wait_time = pc.getInpn();
        }
        else if(pc.inp == "4")
        {
            menu m_difficulty_settings = {
                "\nDifficulty Settings\n\n",
                "Enter a number to choose an option:\n",
                3};
                m_difficulty_settings.options.push_back("");
                m_difficulty_settings.options.push_back("Raise/lower difficulty level");
                m_difficulty_settings.options.push_back("Toggle Survival Mode");

                m_difficulty_settings.display();
                pc.inp = pc.getInps();

            if (pc.inp == "1") {
                ostringstream oss;
                oss.precision(1);

                menu m_difficulties = {
                    "\nDifficulty Levels:\n",
                    "\nRaising the difficulty level increases enemy stats as well as hunger and thirst depletion rate.\nNegative status effects will last twice as long, and deal more damage.\n\n",
                    12};
                m_difficulties.options.push_back("");
                for (int i = 0; i < 11; i++){
                        //Convert floating points to strings
                        oss << fixed << diff_scales[i];
                        string diff_as_str = oss.str();
                        m_difficulties.options.push_back(diff_lvls[i] + " \t\t| Scale: " + diff_as_str + "x");
                        //clear oss buffer
                        oss.str("");
                        oss.clear();
                }

                m_difficulties.display();

                int n_level = 0;
                string level = "";

                cout << "\nCurrent difficulty level " << game_settings.diff_lvl << ": " << game_settings.diff_desc << " (scaling factor of " << game_settings.diff_scale << "x)\n\n";

                cout << "Select new difficulty level:\n>";
                cin >> level;

                if ( isNumber(level) ) {
                    n_level = (stoi(level) - 1);
                    game_settings.diff_desc = diff_lvls[n_level];
                    game_settings.diff_lvl = n_level;
                    game_settings.diff_scale = diff_scales[n_level];
                }

                else {
                    for (int i = 0; i < 11; i++){
                        if ( diff_lvls[i] == strCase(level, "upper" ) ){
                            n_level = i;
                            game_settings.diff_desc = diff_lvls[n_level];
                            game_settings.diff_lvl = n_level;
                            game_settings.diff_scale = diff_scales[n_level];
                            break;
                        }
                    }
                }

                cout << "Your new difficulty level is " << game_settings.diff_lvl+1 << ": " << game_settings.diff_desc << "\n";

                if (game_settings.diff_lvl > 5) prln("Difficulty above normal; survival mode turned on by default.\n(This cannot be changed unless a lower difficulty is selected.)");
            }

            else if (pc.inp == "2") {
                if (!game_settings.survival){
                    prln("Turning on survival mode will require that your character\neat and drink to remain alive; hunger and thirst can be found under the stats menu.");
                    prln("Turn on Survival Mode?");
                    if ( inpAffirm( pc.getInps() ) ) { prln("You have turned on survival mode."); game_settings.survival = true; }
                }
                else {
                    prln("Survival mode is currently active. Turn off?");
                    if ( inpAffirm( pc.getInps() ) )
                        if (game_settings.diff_lvl > 5) prln("Game is currently in hard mode; to turn off survival, please lower the difficulty.");
                        else { prln("Turned off survival mode."); game_settings.survival = false; }
                }
            }
        }
        else if(pc.inp=="5"){return;}
        else {cout<<"Please try again, or press '5' to return.\n";}
    }while(pc.inp!="5");
}


void qSet(int setTo, string name)
{
    int x;
    for(x=0; x<20; x++){
        if (nlist[x].name==name)
            break;
    }
    nlist[x].q.lvl=setTo;
}

int searchinv(int num)
{
    //Search inventory for a specific item by ID number
    //Returns inventory slot of first match found (or '0' if none)
	int x=0;
	do{x++;}while(pc.inv[x].id!=eq[num].id&&x<12);
	if(x<11) return x;
	else return 0;
}

int searchinvtype(string type)
{
    //Search inventory for a type of item
    //Returns inventory slot of first match found (or '0' if none)
	int x=0;
	do{x++;}while(pc.inv[x].type!=type&&x<12);
	if(x<11) return x;
	else return 0;
}

int searchinvsubt(string subtype)
{
	int x=0;
	do {x++;}while(pc.inv[x].subt!=subtype&&x<12);
	if(x<11)return x;
	else return 0;
}

bool checkeq(int slot, string stat, string arg)
{
    //Check equipped items for a particular attribute
    //Returns true or false

	equipment tempeq;
	bool match=false;
	switch(slot)
	{
		case 1: tempeq=pc.weap; break;
		case 2: tempeq=pc.arm; break;
		case 3: tempeq=pc.acc; break;
		default: tempeq=pc.inv[1];
	}

	if(stat=="name") if(tempeq.name==arg) match=true;
	if(stat=="mat") if(tempeq.mat==arg) match=true;
	if(stat=="ench") if(tempeq.ench==arg) match=true;
	if(stat=="type") if(tempeq.type==arg) match=true;
	if(stat=="subt") if(tempeq.subt==arg) match=true;

	return match;
}

void initializeGame(string load_type)
{
    //Load all values of all things
    //Hierarchy here is important
    cout << "Initializing difficulty levels..."; game_settings.initialize_difficulties(); cout << "complete!\n";
    cout << "Setting base difficulty..."; game_settings.set_diff_defaults(); cout << "complete!\n";
    cout << "Load equipment: "; define_equipment();  cout<<"complete\n";
	cout << "Load side quests: "; define_quests(); cout<<"complete\n";
	cout << "Load main quests: "; defineMainQuests(); cout<<"complete\n";
	cout << "Load races: "; define_races(); cout<<"complete\n";
	cout << "Load NPCs: "; define_npcs(); cout<<"complete\n";
	cout << "Load vital NPCs: "; defineMainNPCs(); cout<<"complete\n";
	cout << "Load other NPCs: "; defineCompNPCs(); cout<<"complete\n";
	cout << "Load party: "; define_party(); cout<<"complete\n";
	cout << "Load shops: "; define_shops(); cout<<"complete\n";
	cout << "Load objects: "; define_objects(); cout<<"complete\n";
	cout << "Load buildings: "; define_buildings(); cout<<"complete\n";
	cout << "Load wildlife: "; define_wildlife(); cout<<"complete\n";
	cout << "Load weather: "; define_weather(); cout<<"complete\n";
	cout << "Load seasons: "; define_seasons(); cout<<"complete\n";
	cout << "Load flora: "; define_flora(); cout<<"complete\n";
	cout << "Load map: "; define_areas(); cout<<"complete\n";
	cout << "Load spells: "; define_spells(); cout<<"complete\n";
	cout << "Load songs: "; define_songs(); cout<<"complete\n";
	if (load_type != "load") {cout<<"Load enemies: "; define_enemies(); cout<<"complete\n";}
        else cout << "Load enemies: already loaded!\n";
    cout<<"Load trophies: "; defineTrophies(); cout<<"complete\n\n\n";

    //Initialize randomizer to draw from system clock
	srand(time(NULL));
}

void initializePlayer()
{
	//set inventory
	for(int x=0;x<11;x++)
		pc.inv[x]=eq[0];
    //Player starts with a wooden dagger and woolen cloak in inventory
	pc.inv[1]=eq[2];
	pc.inv[2]=eq[80];
	//Set weapon, accessory items to empty
	pc.acc=eq[0];
	pc.weap=eq[0];
	//Player begins with a cloth tunic for armor
	//
	pc.arm=eq[1];
	pc.sheathed=eq[0];
	//Potions and gold (1 each, 5 gold)
	pc.hpot=1;
	pc.mpot=1;
	pc.gp=5;
    //Player karma starts just above neutral, or 'liked'
    //Helping in random encounters raises karma
    //Refusing or failing to help lowers it
	pc.karma=20;

	//Set base player stats
	pc.status="none";
	pc.hunger = 100; pc.thirst = 100;
	pc.hp=20;
	pc.hpmax=20;
	pc.mp=0;
	pc.mpmax=0;
	pc.atk=1;
	pc.def=0;
	pc.str=1;
	pc.dex=1;
	pc.intl=1;
	pc.lck=1;
	pc.lvl=1;
	pc.xp=0;
	pc.xpnxt=10;
	pc.tat="none";

	//Set survival skills, all starting at 1 (except enchanting)
	pc.wclvl=1;
	pc.fmlvl=1;
	pc.flvl=1;
	pc.clvl=1;
	pc.smlvl=1;
	pc.mlvl=1;
	pc.cklvl=1;
	pc.hlvl=1;
	pc.enchlvl=0;
	pc.swlvl=1;
	pc.frod=0;

	//companion slot set to empty companion object
	pc.comp=empty1;

	//set starting location
	//Plants, animals, and buildings set to null objects
	pc.area=loc[1][10];
	pc.area.loc_flora=plant[0];
	pc.area.loc_fauna=anim[0];
	pc.area.loc_bldg=null_bldg;

	//set first random quest ("Tom")
	pc.area.npcs[2]=cast_npc[2];
    pc.area.npcs[2].q.setProcQuest();
    pc.area.npcs[2].appendQ();
}

void createSaveFolder(string folder_name)
{
    fs::path path = fs::path("Saves") / folder_name;
    string s_path = path.string();
    if ( !fs::exists(path) ) {
        fs::create_directories(path);
        if ( admin ) prln("New folder created at " + s_path);
    }
    else if ( admin ) prln("Folder at " + s_path + " already exists.");
}

fs::path sv_setToFilePath(string file_name, string folder_name)
{
    fs::path path = fs::path("Saves") / folder_name / file_name;
    return path;
}

void savePlayerData(string fname)
{
    ofstream save_player_data;
    string pd_fname = "tsfsav_" + fname + "_pdata.txt";
    fs::path pd_fpath = sv_setToFilePath( pd_fname, fname );

    save_player_data.open(pd_fpath);
    if( !save_player_data.is_open() ) prln("File error: could not open player data");

    save_player_data << pc.name << "\n";
    save_player_data << pc.race << "\n";
    save_player_data << pc.clas << "\n";
    save_player_data << pc.lvl << "\n";
    save_player_data << pc.xp << "\n";
    save_player_data << pc.xpnxt << "\n";
    save_player_data << pc.hp << "\n";
    save_player_data << pc.hpmax << "\n";
    save_player_data << pc.mp << "\n";
    save_player_data << pc.mpmax << "\n";
    save_player_data << pc.atk << "\n";
    save_player_data << pc.def << "\n";
    save_player_data << pc.str << "\n";
    save_player_data << pc.dex << "\n";
    save_player_data << pc.intl << "\n";
    save_player_data << pc.lck << "\n";
    save_player_data << pc.karma << "\n";
    save_player_data << pc.gp << "\n";

    save_player_data << pc.frod << "\n";
    save_player_data << pc.bait << "\n";
    save_player_data << pc.ham << "\n";
    save_player_data << pc.pick << "\n";
    save_player_data << pc.chis << "\n";
    save_player_data << pc.need << "\n";
    save_player_data << pc.shears << "\n";
    save_player_data << pc.axe << "\n";
    save_player_data << pc.tbox << "\n";
    save_player_data << pc.sheath << "\n";
    save_player_data << pc.mlvl << "\n";
    save_player_data << pc.smlvl << "\n";
    save_player_data << pc.clvl << "\n";
    save_player_data << pc.wclvl << "\n";
    save_player_data << pc.fmlvl << "\n";
    save_player_data << pc.cklvl << "\n";
    save_player_data << pc.hlvl << "\n";
    save_player_data << pc.swlvl << "\n";
    save_player_data << pc.enchlvl << "\n";
    save_player_data << pc.frglvl << "\n";
    save_player_data << pc.muslvl << "\n";
    save_player_data << pc.litlvl << "\n";

    //load character description

    save_player_data << pc.sex << "\n";
    save_player_data << pc.age << "\n";
    save_player_data << pc.height << "\n";
    save_player_data << pc.wt << "\n";
    save_player_data << pc.build << "\n";
    save_player_data << pc.hairc << "\n";
    save_player_data << pc.eyec << "\n";
    save_player_data << pc.skinc << "\n";
    save_player_data << pc.tat << "\n";

    save_player_data << pc.hunger << "\n";
    save_player_data << pc.thirst << "\n";

    save_player_data.close();
    if ( save_player_data.is_open() ) prln("File error: could not close player data");
    if (admin || pc.inp != "qsave") prln("Saved player data");
}

void saveAreaData(string fname)
{
    ofstream save_area_data;
	string ad_fname = "tsfsav_" + fname + "_adata.txt";
	fs::path ad_fpath = sv_setToFilePath( ad_fname, fname );

	save_area_data.open(ad_fpath);
	if( !save_area_data.is_open() ) prln("File error: could not load area data");

    //load exploration data
    for(int x=1;x<11;x++)
    {
        for(int y=1;y<11;y++)
        {
            save_area_data << loc[x][y].visits << "\n";

            for (int z = 0; z < 2; z++)
            {
                save_area_data << loc[x][y].npcs[z].q.lvl << "\n";
                save_area_data << loc[x][y].npcs[z].q.comp << "\n";
            }
        }
    }

    save_area_data.close();
    if( save_area_data.is_open() ) prln("File error: could not close area data");

    if (admin || pc.inp != "qsave") cout << "Area data saved.\n";
}

void saveTrophyData(string fname)
{
    ofstream save_trophy_data;
    string td_fname = "tsfsav_" + fname + "_tdata.txt";
    fs::path td_fpath = sv_setToFilePath( td_fname, fname );

    save_trophy_data.open(td_fpath);
    if( !save_trophy_data.is_open() ) {prln("File error: could not load trophy data");}

    for (int i = 0; i < 99; i ++)
    {
        save_trophy_data << trophy_list[i].unlock << "\n";
    }

    save_trophy_data.close();
    if ( save_trophy_data.is_open() ) {prln("File error: could not close trophy data"); }

    if (admin || pc.inp != "qsave") cout << "Trophy data saved.\n";
}

void saveKillData(string fname)
{
    ofstream save_kill_data;
    string kd_fname = "tsfsav_" + fname + "_kdata.txt";
    fs::path kd_fpath = sv_setToFilePath( kd_fname, fname );

    save_kill_data.open(kd_fpath);
    if( !save_kill_data.is_open() ) {prln("File error: could not load kill data");}

    for (int i = 0; i < 46; i++){
        save_kill_data << elist[i].kills << "\n";
    }
    save_kill_data.close();
    if ( save_kill_data.is_open() ) {prln("File error: could not close kill data"); }

    if (admin || pc.inp != "qsave") cout << "kill data saved.\n";
}

void saveCustomCommands(string fname)
{
    ofstream save_commands;
    string cmd_fname = "tsfsav_" + fname + "_commands.txt";
    fs::path cmd_fpath = sv_setToFilePath( cmd_fname, fname );

    save_commands.open(cmd_fpath);
    if( !save_commands.is_open() ) {prln("File error: could not load commands");}

    save_commands << pc.custom_commands2.size() << "\n";

    for (int i = 0; i < pc.custom_commands2.size(); i++){
        save_commands << pc.custom_commands2[i].key << "\n";
        save_commands << pc.custom_commands2[i].command << "\n";
    }
    save_commands.close();
    if ( save_commands.is_open() ) {prln("File error: could not close commands"); }

    if (admin || pc.inp != "qsave") cout << "command data saved.\n";
}

void saveCompanion(string fname)
{
    ofstream save_comp;
    string comp_fname = "tsfsav_" + fname + "_comp.txt";
    fs::path comp_fpath = sv_setToFilePath( comp_fname, fname );

    save_comp.open(comp_fpath);
    if( !save_comp.is_open() ) { prln("File error: could not load companion"); }

    //Save companion stats
    save_comp<<pc.comp.name<<"\n";
    save_comp<<pc.comp.race<<"\n";
    save_comp<<pc.comp.clas<<"\n";
    save_comp<<pc.comp.ai<<"\n";
    save_comp<<pc.comp.lvl<<"\n";
    save_comp<<pc.comp.xp<<"\n";
    save_comp<<pc.comp.xpnxt<<"\n";
    save_comp<<pc.comp.hp<<"\n";
    save_comp<<pc.comp.hpmax<<"\n";
    save_comp<<pc.comp.mp<<"\n";
    save_comp<<pc.comp.mpmax<<"\n";
    save_comp<<pc.comp.atk<<"\n";
    save_comp<<pc.comp.def<<"\n";
    save_comp<<pc.comp.str<<"\n";
    save_comp<<pc.comp.dex<<"\n";
    save_comp<<pc.comp.intl<<"\n";
    save_comp<<pc.comp.lck<<"\n";

    //save_comp equipment
    save_comp<<pc.comp.weap.mat<<"\n";
    save_comp<<pc.comp.weap.type<<"\n";
    save_comp<<pc.comp.weap.ench<<"\n";
    save_comp<<pc.comp.weap.atkb<<"\n";
    save_comp<<pc.comp.weap.defb<<"\n";
    save_comp<<pc.comp.weap.strb<<"\n";
    save_comp<<pc.comp.weap.dxb<<"\n";
    save_comp<<pc.comp.weap.intb<<"\n";
    save_comp<<pc.comp.weap.lckb<<"\n";
    save_comp<<pc.comp.weap.matb<<"\n";
    save_comp<<pc.comp.weap.enchb<<"\n";
    save_comp<<pc.comp.weap.wt<<"\n";
    save_comp<<pc.comp.weap.price<<"\n";
    save_comp<<pc.comp.weap.id<<"\n";

    save_comp<<pc.comp.arm.mat<<"\n";
    save_comp<<pc.comp.arm.type<<"\n";
    save_comp<<pc.comp.arm.ench<<"\n";
    save_comp<<pc.comp.arm.atkb<<"\n";
    save_comp<<pc.comp.arm.defb<<"\n";
    save_comp<<pc.comp.arm.strb<<"\n";
    save_comp<<pc.comp.arm.dxb<<"\n";
    save_comp<<pc.comp.arm.intb<<"\n";
    save_comp<<pc.comp.arm.lckb<<"\n";
    save_comp<<pc.comp.arm.matb<<"\n";
    save_comp<<pc.comp.arm.enchb<<"\n";
    save_comp<<pc.comp.arm.wt<<"\n";
    save_comp<<pc.comp.arm.price<<"\n";
    save_comp<<pc.comp.arm.id<<"\n";

    save_comp<<pc.comp.acc.mat<<"\n";
    save_comp<<pc.comp.acc.type<<"\n";
    save_comp<<pc.comp.acc.ench<<"\n";
    save_comp<<pc.comp.acc.atkb<<"\n";
    save_comp<<pc.comp.acc.defb<<"\n";
    save_comp<<pc.comp.acc.strb<<"\n";
    save_comp<<pc.comp.acc.dxb<<"\n";
    save_comp<<pc.comp.acc.intb<<"\n";
    save_comp<<pc.comp.acc.lckb<<"\n";
    save_comp<<pc.comp.acc.matb<<"\n";
    save_comp<<pc.comp.acc.enchb<<"\n";
    save_comp<<pc.comp.acc.wt<<"\n";
    save_comp<<pc.comp.acc.price<<"\n";
    save_comp<<pc.comp.acc.id<<"\n";

    save_comp<<pc.comp.inv.mat<<"\n";
    save_comp<<pc.comp.inv.type<<"\n";
    save_comp<<pc.comp.inv.ench<<"\n";
    save_comp<<pc.comp.inv.atkb<<"\n";
    save_comp<<pc.comp.inv.defb<<"\n";
    save_comp<<pc.comp.inv.strb<<"\n";
    save_comp<<pc.comp.inv.dxb<<"\n";
    save_comp<<pc.comp.inv.intb<<"\n";
    save_comp<<pc.comp.inv.lckb<<"\n";
    save_comp<<pc.comp.inv.matb<<"\n";
    save_comp<<pc.comp.inv.enchb<<"\n";
    save_comp<<pc.comp.inv.wt<<"\n";
    save_comp<<pc.comp.inv.price<<"\n";
    save_comp<<pc.comp.inv.id<<"\n";

    save_comp.close();
    if ( save_comp.is_open() ) { prln("File error: could not close companion"); }

    if (admin || pc.inp != "qsave") cout << "companion data saved.\n";
}

void save()
{
    string sname;
    ofstream save;
    ofstream recent_saves;

    //get date and time for saves
    time_t now = time(0);
    char* dt = ctime(&now);

    //Get file name from player, or set to player name if quicksaving
    if(pc.inp != "qsave"){
        cout << "Name your save file: ";
        cin >> sname; }
    else sname = pc.name;
    //Check for and create a player save folder
    //Default filepath is ..(root)/saves/player name
    createSaveFolder( sname );

    //Add filename to recent saves list
    //Only triggers if not an autosave or quicksave
    if ( pc.inp != "qsave" ){
        recent_saves.open("recent_saves.txt", std::ios::app);
        if(!recent_saves.is_open()){cout<<"File error: could not load recent saves.\n";}
        recent_saves<<sname<<" - "<<dt;
        recent_saves.close();
        if(recent_saves.is_open()){cout<<"File error: could not close recent saves.\n";} }

    //After adding save name to list, truncate save to exact filepath
    string s_main_fname = "tsfsav_" + sname + ".txt";
    fs::path sv_main_fpath = sv_setToFilePath(s_main_fname, sname);

    save.open( sv_main_fpath );
    if(!save.is_open()){cout<<"File error: could not open save file.\n"; return;}

    /*save character stats*/
    savePlayerData(sname);
    saveTrophyData(sname);
    saveKillData(sname);
    saveCustomCommands(sname);
    saveCompanion(sname);

    //Save potions
    save << pc.hpot << "\n";
    save << pc.mpot << "\n";
    save << pc.apot << "\n";

    //save equipment
    save << pc.weap.mat<<"\n";
    save << pc.weap.type<<"\n";
    save << pc.weap.ench<<"\n";
    save << pc.weap.atkb<<"\n";
    save << pc.weap.defb<<"\n";
    save<<pc.weap.strb<<"\n";
    save<<pc.weap.dxb<<"\n";
    save<<pc.weap.intb<<"\n";
    save<<pc.weap.lckb<<"\n";
    save<<pc.weap.matb<<"\n";
    save<<pc.weap.enchb<<"\n";
    save<<pc.weap.wt<<"\n";
    save<<pc.weap.price<<"\n";
    save<<pc.weap.id<<"\n";

    save<<pc.arm.mat<<"\n";
    save<<pc.arm.type<<"\n";
    save<<pc.arm.ench<<"\n";
    save<<pc.arm.atkb<<"\n";
    save<<pc.arm.defb<<"\n";
    save<<pc.arm.strb<<"\n";
    save<<pc.arm.dxb<<"\n";
    save<<pc.arm.intb<<"\n";
    save<<pc.arm.lckb<<"\n";
    save<<pc.arm.matb<<"\n";
    save<<pc.arm.enchb<<"\n";
    save<<pc.arm.wt<<"\n";
    save<<pc.arm.price<<"\n";
    save<<pc.arm.id<<"\n";

    save<<pc.acc.mat<<"\n";
    save<<pc.acc.type<<"\n";
    save<<pc.acc.ench<<"\n";
    save<<pc.acc.atkb<<"\n";
    save<<pc.acc.defb<<"\n";
    save<<pc.acc.strb<<"\n";
    save<<pc.acc.dxb<<"\n";
    save<<pc.acc.intb<<"\n";
    save<<pc.acc.lckb<<"\n";
    save<<pc.acc.matb<<"\n";
    save<<pc.acc.enchb<<"\n";
    save<<pc.acc.wt<<"\n";
    save<<pc.acc.price<<"\n";
    save<<pc.acc.id<<"\n";

    //save inventory

    for(int x=1;x<11;x++)
    {
    save<<pc.inv[x].mat<<"\n";
    save<<pc.inv[x].type<<"\n";
    save<<replace_spaces(pc.inv[x].ench)<<"\n";
    save<<pc.inv[x].atkb<<"\n";
    save<<pc.inv[x].defb<<"\n";
    save<<pc.inv[x].strb<<"\n";
    save<<pc.inv[x].dxb<<"\n";
    save<<pc.inv[x].intb<<"\n";
    save<<pc.inv[x].lckb<<"\n";
    save<<pc.inv[x].matb<<"\n";
    save<<pc.inv[x].enchb<<"\n";
    save<<pc.inv[x].wt<<"\n";
    save<<pc.inv[x].price<<"\n";
    save<<pc.inv[x].id<<"\n";
    }

    //save spells
    for(int x=0;x<70;x++)
    {
        save<<splist[x].unlock<<"\n";
    }

    save<<pc.area.x<<"\n";
    save<<pc.area.y<<"\n";

    //Visits, NPC data (quests)
    saveAreaData(sname);

    //save settings
    save<<game_settings.sound<<"\n";
    save<<game_settings.display_type<<"\n";
    save<<game_settings.pcol_1<<"\n";
    save<<game_settings.pcol_2<<"\n";
    save<<game_settings.mtcol<<"\n";
    save<<game_settings.psym_1<<"\n";
    save<<game_settings.psym_2<<"\n";

    save<<m_data.lost_in_party<<"\n";
    save<<m_data.lostx<<"\n";
    save<<m_data.losty<<"\n";
    save<<pc.fairy_missive<<"\n";
    save<<m_data.fairy_loc_x<<"\n";
    save<<m_data.fairy_loc_y<<"\n";
    save<<m_data.ferry<<"\n";
    save<<m_data.bridge<<"\n";
    save<<m_data.lost_village<<"\n";
    save<<m_data.dwarven_city<<"\n";

    save<<lost.adj<<"\n";
    save<<lost.age<<"\n";
    save<<lost.mf<<"\n";
    save<<lost.n<<"\n";

    //Environmental data (time, day, season)
    save<<timen<<"\n";
    save<<day<<"\n";
    save<<curs.name<<"\n";

    //Difficulty
    save<<game_settings.diff_lvl<<"\n";
    save<<game_settings.survival<<"\n";

    save.close();
    if (save.is_open()){cout<<"File error: could not close save file.\n";}
    if ( pc.inp != "qsave" ) cout << "\nGame Saved\n";
}

void autosave()
{
    //Saves automatically without prompting for save type
    pc.inp = "qsave";
    save();
    pc.inp = "null";
}

void loadPlayerData(string fname)
{
    ifstream load_player_data;
    string pd_fname = "tsfsav_"+fname+"_pdata.txt";
    fs::path pd_fpath = sv_setToFilePath( pd_fname, fname );

    load_player_data.open(pd_fpath);

    load_player_data>>pc.name;
    load_player_data>>pc.race;
    load_player_data>>pc.clas;
    load_player_data>>pc.lvl;
    load_player_data>>pc.xp;
    load_player_data>>pc.xpnxt;
    load_player_data>>pc.hp;
    load_player_data>>pc.hpmax;
    load_player_data>>pc.mp;
    load_player_data>>pc.mpmax;
    load_player_data>>pc.atk;
    load_player_data>>pc.def;
    load_player_data>>pc.str;
    load_player_data>>pc.dex;
    load_player_data>>pc.intl;
    load_player_data>>pc.lck;
    load_player_data>>pc.karma;
    load_player_data>>pc.gp;

    load_player_data>>pc.frod;
    load_player_data>>pc.bait;
    load_player_data>>pc.ham;
    load_player_data>>pc.pick;
    load_player_data>>pc.chis;
    load_player_data>>pc.need;
    load_player_data>>pc.shears;
    load_player_data>>pc.axe;
    load_player_data>>pc.tbox;
    load_player_data>>pc.sheath;
    load_player_data>>pc.mlvl;
    load_player_data>>pc.smlvl;
    load_player_data>>pc.clvl;
    load_player_data>>pc.wclvl;
    load_player_data>>pc.fmlvl;
    load_player_data>>pc.cklvl;
    load_player_data>>pc.hlvl;
    load_player_data>>pc.swlvl;
    load_player_data>>pc.enchlvl;
    load_player_data>>pc.frglvl;
    load_player_data >> pc.muslvl;
    load_player_data >> pc.litlvl;

    //load_player_data character description

    load_player_data>>pc.sex;
    load_player_data>>pc.age;
    load_player_data>>pc.height;
    load_player_data>>pc.wt;
    load_player_data>>pc.build;
    load_player_data>>pc.hairc;
    load_player_data>>pc.eyec;
    load_player_data>>pc.skinc;
    load_player_data>>pc.tat;

    load_player_data>>pc.hunger;
    load_player_data>>pc.thirst;

    load_player_data.close();
}

void loadAreaData(string fname)
{
    ifstream load_area_data;
	string ad_fname = "tsfsav_"+fname+"_adata.txt";
	fs::path ad_fpath = sv_setToFilePath( ad_fname, fname );

	load_area_data.open(ad_fpath);
	if( !load_area_data.is_open() ) prln("File error: could not load area data");

    //load exploration data
    for(int x=1;x<11;x++)
    {
        for(int y=1;y<11;y++)
        {
            load_area_data >> loc[x][y].visits;

            for (int z = 0; z < 2; z++)
            {
                load_area_data >> loc[x][y].npcs[z].q.lvl;
                load_area_data >> loc[x][y].npcs[z].q.comp;

                //Load NPC quest level data into indexed quest items
                qlog[ loc[x][y].npcs[z].q.id -1 ].lvl = loc[x][y].npcs[z].q.lvl;
            }
        }
    }

    load_area_data.close();
    if( load_area_data.is_open() ) prln("File error: could not close area data");
}

void loadTrophyData(string fname)
{
    ifstream load_trophy_data;
    string td_fname = "tsfsav_" + fname + "_tdata.txt";
    fs::path td_fpath = sv_setToFilePath( td_fname, pc.name );
    string s_td_fpath = td_fpath.string();

    load_trophy_data.open(td_fpath);
    if( !load_trophy_data.is_open() ) {prln("File error: could not load trophy data");}

    for (int i = 0; i < 99; i ++)
    {
        load_trophy_data >> trophy_list[i].unlock;
    }

    load_trophy_data.close();
    if ( load_trophy_data.is_open() ) {prln("File error: could not close trophy data"); }
}

void loadKillData(string fname)
{
    ifstream load_kill_data;
    string kd_fname = "tsfsav_" + fname + "_kdata.txt";
    fs::path kd_fpath = sv_setToFilePath( kd_fname, fname );
    string s_kd_fpath = kd_fpath.string();

    load_kill_data.open(kd_fpath);
    if( !load_kill_data.is_open() ) {prln("File error: could not load " + s_kd_fpath);}

    for (int i = 0; i < 46; i ++) { load_kill_data >> elist[i].kills; }

    load_kill_data.close();
    if ( load_kill_data.is_open() ) {prln("File error: could not close kill data"); }
}

void loadCustomCommands(string fname)
{
    ifstream load_commands;
    string cmd_fname = "tsfsav_" + fname + "_commands.txt";
    fs::path cmd_fpath = sv_setToFilePath( cmd_fname, fname );
    string s_cmd_fpath = cmd_fpath.string();

    load_commands.open(cmd_fpath);
    if( !load_commands.is_open() ) {prln("File error: could not load " + s_cmd_fpath);}

    int list_size;
    load_commands >> list_size;

    for (int i = 0; i < list_size; i++)
    {
        custom_c2 temp = {"", ""};
        load_commands >> temp.key;
        load_commands >> temp.command;
        pc.custom_commands2.push_back(temp);
    }

    load_commands.close();
    if ( load_commands.is_open() ) { prln("File error: could not close command data"); }
}

void loadCompanion(string fname)
{
    ifstream load_comp;
    string comp_fname = "tsfsav_" + fname + "_comp.txt";
    fs::path comp_fpath = sv_setToFilePath( comp_fname, fname );
    string s_comp_fpath = comp_fpath.string();

    load_comp.open(comp_fpath);
    if( !load_comp.is_open() ) {prln("File error: could not load " + s_comp_fpath);}

    //Load companion

    load_comp>>pc.comp.name;
    load_comp>>pc.comp.race;
    load_comp>>pc.comp.clas;
    load_comp>>pc.comp.ai;
    load_comp>>pc.comp.lvl;
    load_comp>>pc.comp.xp;
    load_comp>>pc.comp.xpnxt;
    load_comp>>pc.comp.hp;
    load_comp>>pc.comp.hpmax;
    load_comp>>pc.comp.mp;
    load_comp>>pc.comp.mpmax;
    load_comp>>pc.comp.atk;
    load_comp>>pc.comp.def;
    load_comp>>pc.comp.str;
    load_comp>>pc.comp.dex;
    load_comp>>pc.comp.intl;
    load_comp>>pc.comp.lck;

    //save equipment
    load_comp>>pc.comp.weap.mat;
    load_comp>>pc.comp.weap.type;
    load_comp>>pc.comp.weap.ench;
    load_comp>>pc.comp.weap.atkb;
    load_comp>>pc.comp.weap.defb;
    load_comp>>pc.comp.weap.strb;
    load_comp>>pc.comp.weap.dxb;
    load_comp>>pc.comp.weap.intb;
    load_comp>>pc.comp.weap.lckb;
    load_comp>>pc.comp.weap.matb;
    load_comp>>pc.comp.weap.enchb;
    load_comp>>pc.comp.weap.wt;
    load_comp>>pc.comp.weap.price;
    load_comp>>pc.comp.weap.id;

    load_comp>>pc.comp.arm.mat;
    load_comp>>pc.comp.arm.type;
    load_comp>>pc.comp.arm.ench;
    load_comp>>pc.comp.arm.atkb;
    load_comp>>pc.comp.arm.defb;
    load_comp>>pc.comp.arm.strb;
    load_comp>>pc.comp.arm.dxb;
    load_comp>>pc.comp.arm.intb;
    load_comp>>pc.comp.arm.lckb;
    load_comp>>pc.comp.arm.matb;
    load_comp>>pc.comp.arm.enchb;
    load_comp>>pc.comp.arm.wt;
    load_comp>>pc.comp.arm.price;
    load_comp>>pc.comp.arm.id;

    load_comp>>pc.comp.acc.mat;
    load_comp>>pc.comp.acc.type;
    load_comp>>pc.comp.acc.ench;
    load_comp>>pc.comp.acc.atkb;
    load_comp>>pc.comp.acc.defb;
    load_comp>>pc.comp.acc.strb;
    load_comp>>pc.comp.acc.dxb;
    load_comp>>pc.comp.acc.intb;
    load_comp>>pc.comp.acc.lckb;
    load_comp>>pc.comp.acc.matb;
    load_comp>>pc.comp.acc.enchb;
    load_comp>>pc.comp.acc.wt;
    load_comp>>pc.comp.acc.price;
    load_comp>>pc.comp.acc.id;

    load_comp>>pc.comp.inv.mat;
    load_comp>>pc.comp.inv.type;
    load_comp>>pc.comp.inv.ench;
    load_comp>>pc.comp.inv.atkb;
    load_comp>>pc.comp.inv.defb;
    load_comp>>pc.comp.inv.strb;
    load_comp>>pc.comp.inv.dxb;
    load_comp>>pc.comp.inv.intb;
    load_comp>>pc.comp.inv.lckb;
    load_comp>>pc.comp.inv.matb;
    load_comp>>pc.comp.inv.enchb;
    load_comp>>pc.comp.inv.wt;
    load_comp>>pc.comp.inv.price;
    load_comp>>pc.comp.inv.id;

    load_comp.close();
    if ( load_comp.is_open() ) {prln("File error: could not close companion data"); }
}

void load()
{
    string loadn;
    string itname;
    string recent, str;
    vector<string> saves_in_reverse;
    ifstream load;
    ifstream open_saves;

    //display the three most recent saves
    //if there are fewer than three saves, it will show only what there is
    //A good chunk of this is courtesy of ChatGPT. Stay humble.
    open_saves.open("recent_saves.txt");
    if(!open_saves.is_open()){cout<<"Error loading file!\n"; return;}

    cout<<"\nRecent Saves (does not include autosave) :\n";
    while(getline(open_saves, str)){
            saves_in_reverse.insert(saves_in_reverse.begin(), str);
            if (open_saves.eof()) {
            break;}}
    for(int x=0; x<(saves_in_reverse.size() < 3 ? saves_in_reverse.size() : 3); x++){
            cout<<"("<<x+1<<") "<<saves_in_reverse[x]<<"\n";}

    open_saves.close();
    if (open_saves.is_open()) {cout << "Error closing file!" << endl;}

    cout<<"\nEnter the name of the save you wish to load: \n";
    loadn = pc.getInps();

    if ( loadn.length() < 3 && isRange( 1, 3, stoi(loadn) ) ) {
        loadn = truncateSpace( saves_in_reverse[ stoi(loadn) - 1 ] );
    }

    string sv_mainfile = "tsfsav_" + loadn + ".txt";
    debugMsg(loadn, 1);
    fs::path sv_main_fpath = sv_setToFilePath( sv_mainfile, loadn );
    string s_main_fpath = sv_main_fpath.string();

    load.open( sv_main_fpath );
    if(!load.is_open()) {cout<<"File error: could not load " << s_main_fpath << "\n"; return;}

    initializeGame("load");

    loadPlayerData(loadn);
    loadTrophyData(loadn);
    loadKillData(loadn);
    loadCustomCommands(loadn);
    loadCompanion(loadn);

    //Load potions
    load >> pc.hpot;
    load >> pc.mpot;
    load >> pc.apot;

    //load equipment

    load>>pc.weap.mat;
    load>>pc.weap.type;
    load>>pc.weap.ench;
    pc.weap.ench = replace_underscores(pc.weap.ench);
    load>>pc.weap.atkb;
    load>>pc.weap.defb;
    load>>pc.weap.strb;
    load>>pc.weap.dxb;
    load>>pc.weap.intb;
    load>>pc.weap.lckb;
    load>>pc.weap.matb;
    load>>pc.weap.enchb;
    load>>pc.weap.wt;
    load>>pc.weap.price;
    load>>pc.weap.id;
    pc.weap.name=eq[pc.weap.id].name;
    pc.weap.desc=eq[pc.weap.id].desc;

    load>>pc.arm.mat;
    load>>pc.arm.type;
    load>>pc.arm.ench;
    pc.arm.ench = replace_underscores(pc.arm.ench);
    load>>pc.arm.atkb;
    load>>pc.arm.defb;
    load>>pc.arm.strb;
    load>>pc.arm.dxb;
    load>>pc.arm.intb;
    load>>pc.arm.lckb;
    load>>pc.arm.matb;
    load>>pc.arm.enchb;
    load>>pc.arm.wt;
    load>>pc.arm.price;
    load>>pc.arm.id;
    pc.arm.name=eq[pc.arm.id].name;
    pc.arm.desc=eq[pc.arm.id].desc;

    load>>pc.acc.mat;
    load>>pc.acc.type;
    load>>pc.acc.ench;
    pc.acc.ench = replace_underscores(pc.acc.ench);
    load>>pc.acc.atkb;
    load>>pc.acc.defb;
    load>>pc.acc.strb;
    load>>pc.acc.dxb;
    load>>pc.acc.intb;
    load>>pc.acc.lckb;
    load>>pc.acc.matb;
    load>>pc.acc.enchb;
    load>>pc.acc.wt;
    load>>pc.acc.price;
    load>>pc.acc.id;
    pc.acc.name=eq[pc.acc.id].name;
    pc.acc.desc=eq[pc.acc.id].desc;

    //save inventory

    for(int x=1;x<11;x++)
    {
    load>>pc.inv[x].mat;
    load>>pc.inv[x].type;
    load>>pc.inv[x].ench;
    pc.inv[x].ench = replace_underscores(pc.inv[x].ench);
    load>>pc.inv[x].atkb;
    load>>pc.inv[x].defb;
    load>>pc.inv[x].strb;
    load>>pc.inv[x].dxb;
    load>>pc.inv[x].intb;
    load>>pc.inv[x].lckb;
    load>>pc.inv[x].matb;
    load>>pc.inv[x].enchb;
    load>>pc.inv[x].wt;
    load>>pc.inv[x].price;
    load>>pc.inv[x].id;
    pc.inv[x].name=eq[pc.inv[x].id].name;
    pc.inv[x].desc=eq[pc.inv[x].id].desc;
    }

    //load spells
    for(int x=0;x<70;x++)
    {
        load>>splist[x].unlock;
    }

    //load player location
    load>>pc.area.x;
    load>>pc.area.y;
    pc.area=loc[pc.area.x][pc.area.y];
    pc.area.loc_flora=plant[0];
	pc.area.loc_fauna=anim[0];

	//Visits, NPC data (quests)
	loadAreaData(loadn);

    //load settings
    load>>game_settings.sound;
    load>>game_settings.display_type;
    load>>game_settings.pcol_1;
    load>>game_settings.pcol_2;
    load>>game_settings.mtcol;
    load>>game_settings.psym_1;
    load>>game_settings.psym_2;

    load>>m_data.lost_in_party;
    load>>m_data.lostx;
    load>>m_data.losty;
    load>>pc.fairy_missive;
    load>>m_data.fairy_loc_x;
    load>>m_data.fairy_loc_y;
    load>>m_data.ferry;
    load>>m_data.bridge;
    load>>m_data.lost_village;
    load >> m_data.dwarven_city;

    //if player has previously unlocked a path, it's set to open
	if(m_data.ferry) loc[2][6].east=true;
	if(m_data.bridge) loc[5][9].north=true;
	if(m_data.dwarven_city) loc[1][4].east = true;

    load>>lost.adj;
    load>>lost.age;
    load>>lost.mf;
    load>>lost.n;

    lost.initialize(1);

    //Environmental data (time, day, season)
    load>>timen;
    load>>day;
    load>>curs.name;

    load>>game_settings.diff_lvl;
    game_settings.initialize_difficulties();
    game_settings.diff_scale = diff_scales[ game_settings.diff_lvl ];
    game_settings.diff_desc = diff_lvls[ game_settings.diff_lvl ];
    debugMsg(" difficulty level set to ", game_settings.diff_lvl);

    load>>game_settings.survival;

    //Initialize season
    for(int ct = 0; ct < 4; ct++) {if(curs.name == seasons[ct].name) curs = seasons[ct];}

    load.close();//close file
    if(load.is_open()) cout<<"File error: could not close file\n";

    colSet( game_settings.mtcol, pc.area.name + "\n");
    colSet( game_settings.mtcol, pc.area.desc);
}

void setKarma(int k, bool add_to)
{
    pc.karma = k;
    if(add_to) pc.karma++; k++;
    if (pc.karma > 50) pc.karma = 50;
    if (pc.karma < 0) pc.karma = 0;
    switch(k)
    {
        case 0: {pc.karma_lvl = "vilified";} break;
        case 5: {pc.karma_lvl = "hated";} break;
        case 15: {pc.karma_lvl = "disliked";} break;
        case 20: {pc.karma_lvl = "neutral";} break;
        case 30: {pc.karma_lvl = "liked";} break;
        case 40: {pc.karma_lvl = "honored";} break;
        case 50: {pc.karma_lvl = "idolized";} break;
        default: {pc.karma_lvl = "unknown";} break;
    }
}

bool updateSeason(bool force)
{
    bool change_seasons = false;
    //If it's the 13th day of the current season, switch seasons
    //Each season is 13 days, or 130 turns, long
    //Force the switch if needed
    if((day%13==0&&timen==1)||force==true)
    {
        change_seasons = true;
        //Set sound effect for new season
        string sdSeas[4]={sdSpring, sdSummer, sdFall, sdWinter};
        //If season is winter, roll over to spring
        if(curs.id==3) {curs=seasons[0];}
        //Otherwise transition to next season
        else {curs=seasons[curs.id+1];}
        //Play sound effect
        if(game_settings.sound) mciSendString(sdSeas[curs.id].c_str(), NULL, 0, NULL);
        //Affect seasonal changes based on new season (defined in "seasonal.h")
        seasonal_changes(curs.name);
    }
    return change_seasons;
}

void pickup(equipment item)
{
	int x = 0;
	do {x++;} while(pc.inv[x].name != "empty" && x < 11);
	if (x > 10)
    {
        prln("You are out of inventory space!\nYou leave the item on the ground.");
        pc.area.loot = item;
        return;
    }

    if(item.name!="empty")
    {
        //Books, scrolls and recipes are created when they enter the player's inventory:
        if ( item.name == "book" || item.name == "scroll" || item.name == "recipe" ) {
            item.create_book(false, "");
        }

        if( item.id == pc.inv[x].id )
        {
            pc.area.loot=eq[0];
            loc[pc.area.x][pc.area.y].loot=eq[0];
        }

        pc.inv[x] = item;

        cout<<"\nYou put the "<<item.showName()<<" in your pack.\n";

        if(game_settings.sound) mciSendString(sdItem.c_str(), NULL, 0, NULL);
    }
    else cout<<"There's nothing here!\n";
}

void note(int num)
{
	string notein;
	if(num==1)
	{
	cout<<"What would you like to make a note of?\n";
	getline(cin, notein);
	notes+=notein+"\n";
	}
	else
		cout<<notes;
}

void create_shop(int shopID, string shopN)
{
    //0 - bakery
    //1 - fencer
    shop temp;
    int choice;
    vector<equipment> wares;

    if(shopN!="null")
    {
        for(int x=0;x<sizeof(shops);x++)
        {if(shops[x].name==shopN) {temp=shops[x]; break;}}
    }
    else temp=shops[shopID];
    int exit=temp.wares_max;

    cout<<"\n";
    colSet(240, temp.name);
    cout<<"\n\n"<<temp.desc<<"\n";


//Add a blank item so list starts at 1
    wares.push_back(eq[0]);
    //Populate wares
    for(int x=0; x<temp.wares_id.size(); x++)
    {
       if(eq[temp.wares_id[x]].id!=0) wares.push_back(eq[temp.wares_id[x]]);
    }

    //Main buy loop
    do
    {
        cout<<temp.greeting;
        cout<<"\nGold: "<<pc.gp<<"\n";
        cout <<"\n\n  Item\t\t\t|  Price\n";
	    cout <<"+-----------------------+-------------+\n";
        for(int x=1;x<temp.wares_max;x++)
        {
            cout<<"["<<x<<"] --- ";
            cout<<wares[x].showName(); if(wares[x].mat == "none") cout << "\t";
            cout << "\t| "<<wares[x].price<<"gp\n";//output wares menu
        }
        cout<<"["<<exit<<"] Exit\n";
        prln("Enter a number to select an item:");

        choice=check_int();

        if(choice<exit)
        {
        if(pc.gp<wares[choice].price) cout<<"\nYou don't have enough gold!\n";
        else
        {
            pickup(wares[choice]);
            pc.gp-=wares[choice].price;
        }//has enough gold
        }//chose a menu option
        else if(choice==exit)
        {
            prln(temp.leave);
        }//chose to leave
        else prln(temp.error);
    }while(choice!=exit);
}

void print_env_options()
{
    int x, y, z;
    area temp;
    cout<<"Enter x and y coordinates to see environmental data for that area.\nEnter X: ";
    x=check_int();
    cout<<"Enter Y: ";
    y=check_int();

    temp=loc[x][y];

    cout<<temp.name<<"\n";
    cout<<"ID: "<<temp.id<<"\n";
    for(z=0; z<=temp.flora_types.size(); z++)
    {
        cout<<"Flora "<<z<<": "<<temp.flora_types[z].name<<"\n";
    }
}

void aria_sings()
{
    string songs[5]={"an enchanting", "a haunting", "a comforting", "an energetic", "a wistful"};
    cout<<"\n\nAria sings "<<songs[rand()%5]<<" melody.\n\n";

    int hpg = roll(5);
    int stat = roll(2);
    int chance = rollfor(1, 3);

    if(chance) switch (stat)
    {
        case 1: {cout <<"You feel a little better.\n"; pc.hp+=hpg;} break;
        case 2: {cout <<"You feel a little better.\n"; pc.mp += hpg;} break;
    }

}

void smith()
{
    if ( !pc.area.smithy ) {cout << "You need a smithy nearby to forge metal equipment.\n"; return;}
    if ( !pc.ham ) {cout << "You need a hammer to smith anything.\n"; return;}

    int sm_item_id[32] = {0, 3, 0, 138, 7, 0, 0, 10, 8, 37, 0, 24, 0, 95, 14, 114, 0, 30, 0, 0, 27, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	string smskl[32]=
	{
	    "null",//1
		"dagger",//2
		"",//3
		"spearhead",//4
		"shortsword",//5
		"",//6
		"wakizashi",//7
		"chainmail",//10
		"splintmail",//11
		"steel bar",//12
		"mace",
		"smallshield",
		"falchion",
		"rapier",
		"longsword",
		"katana",
		"",
		"plate-armor",
		"",
		"",
		"kiteshield",
		"",
		"",
		"tower-shield",
		"",
		"greatsword",
		"",
		"battleaxe",
		"",
		"nodachi"};

    int num;

    //Main smithing function
    /* Unlooped because of the length of skill list;
        Player can read the list on 'smith' and thereafter enter 'smith x' to select an item */

    string inp = "null";

        int item_id = 0;
        int skill_id = 0;

        cout << "\nSmithing Level: " << pc.smlvl << "\n";
        cout << "\nItems available:\n";
        cout << "--------------------------\n";

        //Show list of smithing skills up to your current level
        for(int x = 1; (x < 31 && x <= pc.smlvl); x++){
            {if (smskl[x] != "") cout << "[" << x << "] --- " << smskl[x] << "\n";}
        }

        cout << "\nWhich item do you want to create? (enter 'none' to return.)\n";
        inp = pc.getInps();

		for (int i = 0; i < 31; i++)
        {
            //Check for both integer and string input
            if ( (inp.length() < 3 && stoi(inp) == i) || inp == smskl[i] )
            {
                if ( pc.smlvl >= i ) {
                    item_id = sm_item_id[i]; //Target item index ID
                    skill_id = i; //Target item smithing list position
                    break; }
                else {prln("You aren't experienced enough to make that!"); return;}
            }
        }

        if( inp == "none" ) {prln("You decide not to make anything."); return;}
        if( item_id == 0) {prln("You can't make that!");}

        //Find ore in inventory
        int n_mat_1 = searchinvtype("ore");
        if ( !n_mat_1 ) { n_mat_1 = searchinvtype("scale"); if ( !n_mat_1 ) {prln("You need ore to craft with!"); return;} }

        //Create temporary material for handling
        equipment mat_1 = pc.inv[ n_mat_1 ];

        //Check to see if the player has more than one ore
        if ( pc.searchInvTypeFrom("ore", n_mat_1 + 1) )
        {
            prln("What material do you want to use?");
            pc.showInv();
            n_mat_1 = pc.getInpn();
            if( n_mat_1 > 10 || mat_1.type != "ore" ) {prln("There's not enough metal in that to make anything."); return;}
        }

        //For items requiring a second material
        if ( item_id == 37 /*steel bar*/) {
            int coal = searchinv( getItemID("none", "coal") );
            int iron = searchinv( getItemID("iron", "ore") );

            if ( !coal || !iron ) { prln("You need iron and coal to make steel."); return;}

            else
            {
                prln("You stoke the furnace with some extra coal and\nmelt the impurities out of a lump of iron ore.");
                pc.inv[ coal ] = eq[0];
                pc.inv[ iron ] = eq[0];
                pickup( eq[37] );
                return;
            }
        }
        if ( mat_1.mat == "mithril" ) {
            int mat_2 = pc.searchInvFrom("ore", "mithril", n_mat_1);
            if ( !mat_2 ){ prln("You need two mithril ore to craft an item!"); return; }
            else {
                pc.inv[ mat_2] = eq[0];
                prln("You place two chunks of thinly ore-veined rock into the furnace.");
            }
        }
        if ( mat_1.name == "scale" ) {
            int coal = searchinv( getItemID("none", "coal") );
            if( !coal ) { prln("You're going to need some extra coal to melt down\nthe tough dragonhide.\n"); return; }
            else{
                pc.inv[ coal ] = eq[0];
                int timer = rollcap(0, pc.smlvl, 20);
                sleepFor( roll(20 - timer) );
                prln("You stoke the furnace and forge the tough dragonhide into workable ingots.\n");
            }
        }

        //Create item & material
        equipment temp = eq[ item_id ];
        //Add material attributes
        temp.mat = mat_1.mat;
        temp.matb = mat_1.matb;
        temp.ench = mat_1.ench;
        //Add material bonuses to attack, defense, enchantment bonus
        if( temp.type == "weapon" ) temp.atkb += mat_1.matb;
        temp.defb += mat_1.matb;
        temp.enchb += mat_1.enchb;
        //Stat bonuses
        temp.strb += mat_1.strb;
        temp.dxb += mat_1.dxb;
        temp.intb += mat_1.intb;
        temp.lckb += mat_1.lckb;
        //Increase item price by material quality
        temp.price += 10 * ( (temp.matb + temp.enchb) / 2);
        //Remove material from inventory
        pc.inv[ n_mat_1 ] = eq[0];

        prln( "You heat up the forge and smith " + aoranf( temp.showName(), false ) + "." );
        pickup(temp);
        //Add smithing experience
        pc.smlvl += 1;

    if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\chop2.wav", NULL, 0, NULL);
}

enem cast(enem enemy)
{
    //Enemy details are passed here, altered, and returned to combat function
	int mdmg;

	//Used to switch damage step on/off. Default true.
	bool atk=true;

    //If the player's weapon is enchanted halve the casting cost
    if(pc.weap.ench==pc.casting.elem) pc.casting.cost/=2;
    //If the player's weapon is *made of* the casting material...
    if(pc.weap.mat==pc.casting.elem) pc.casting.cost=0;

	//Check if player's MP is at or above the cost of the spell
	if(pc.mp < pc.casting.cost) {prln("You try to summon the energy for a special attack, but you're exhausted!"); return enemy;}
    //Make sure the spell has been unlocked
    if(!pc.casting.unlock) {prln("You don't know how to do that!"); return enemy;}

    //Subtract MP cost from player MP
    pc.mp-=pc.casting.cost;

    //If the spell deals physical damage, measure damage by strength
    if(pc.casting.elem=="sword"||pc.casting.elem=="physical") {mdmg=rand()%((pc.str+pc.casting.dmg)-enemy.def)+1;}
    //Otherwise damage by intellect
    else mdmg=rand()%( (pc.intl+pc.casting.dmg)-(enemy.def+enemy.intl))+1;

		cout<<"\n\nYou cast "<<pc.casting.name<<"!\n";

		//Spell Effects
		//some may have no effect besides basic damage, but still need a description
		switch(pc.casting.id)
		{
			case 0://char
			    cout<<"A lick of flame scorches your enemy.\n";
			break;
			case 1://frostbite
			{
				cout<<"You draw the heat out of your opponent; frost appears across their skin.\n";
			}
			break;
			case 2://shock
			{
				cout<<"A bright arc of energy snaps out from your outstretched hands.\n";
			}
			break;
			case 3://gust
			{
				cout<<"You thrust a palm forward, directing a blast of wind at your opponent.\n";
			}
			break;
			case 4://rockspike
			{
				cout<<"With a forceful motion, you summon a spar of stone from the earth beneath your foe.\n";
			}
			break;
			case 5://pierce
			{
				mdmg+=enemy.def;
				cout<<"Your precise attack pierces through a gap\nin the enemy's defenses.\n";
			}
			break;
			case 6://salve
			{
				cout<<"You daub your wounds with a medicinal salve. You feel a little better.\n";
			}
			break;
			case 7://throw
			{
			    equipment item;

			    pc.showInv();
				cout<<"Throw which item? (inv #, 0 to throw weapon)\n";
				int num = pc.getInpn();

				switch(num)
				{
					case 0:
						item=pc.weap;
						pc.carrywt-=pc.weap.wt;
						pc.area.loot=pc.weap;
						pc.weap=eq[0];
					break;
					case 1 ... 10:
						item=pc.inv[num];
						pc.carrywt-=pc.inv[num].wt;
						pc.area.loot=pc.inv[num];
						pc.inv[num]=eq[0];
					break;
					default:
						cout<<"That's not something you can throw.\n";
					break;
				}
				if(item.name=="empty")
				{
					cout<<"Nothing to throw!\n";
					return enemy;
				}
				else if(item.name=="bottle")
				{

					cout<<"The glass bottle shatters on impact, showering your opponent in ";
					if(item.ench=="poison")
					{
						cout<<"caustic poison.\nThe "<<enemy.name<<" has been poisoned!\n";
						enemy.status="poisoned";
					}
					if(item.subt=="dye")
					{
						cout<<item.ench<<"!\n";
						string color_o=item.ench;
    						string color;

    					//retrieve the color of the dye
    						const int length = color_o.length();
    						int length_n=length-4;
    						color="";
    						for(int y=0;y<length_n; y++)
    						{
        						color=color+color_o.at(y);
   						 }
						enemy.name=color+" "+enemy.name;
					}//if bottle contained dye
					else
					{
						cout<<item.ench<<".\n";
					}
				}
				else
				{
					int stdm=rand()%pc.str+1;
					mdmg=2*(item.wt+stdm)+item.atkb;
					cout<<"You hurl the "<<item.name<<" at your enemy.\n";
					cout<<"It impacts for "<<mdmg<<" damage!\n";
				}
			}
			break;
			case 8://crush
			{
				enemy.def-=1;
				enemy.dex-=1;
				cout<<"Your crushing blow damages the enemy's\narmor and limits their movement!\n";
			}
			break;
			case 9://poison
			{
				cout<<"You quickly poison your weapon and attack;\n";
				bool psch=roll20(pc.dex);
				if(psch==true)
				{
					cout<<"You poison the enemy!";
					enemy.status="poisoned";
				}
				else
					cout<<"The poison fails to enter the wound.\n";
			}
			break;
			case 10://flush
			{
				atk=false;
				pc.status="none";
				cout<<"You flush the impurities from your system,/nrestoring your status.\n";
			}
			break;
			case 11://fireball
			break;
			case 12://mist
			{
				atk=false;
				cout<<"You summon a swirling cloud of mist,\ncloaking your movements and lowering\nthe enemy's accuracy.\n";
				pc.dex+=2;
				enemy.dex-=2;
			}
			break;
			case 13://drench
			{
				cout<<"You hit the enemy with a forceful spray of water.\n";
				bool ch=roll20(2);
				if(ch==true)
				{
					cout<<"The enemy chokes and splutters, taking an extra 1 damage.\n";
					mdmg+=1;
				}
			}
			break;
			case 14://hack
			{
				int critc=rand()%30+1;
				if(pc.lck>=critc)
				{
					mdmg*=3;
					cout<<"Critical hit!\nYou hack off a limb, severely damaging the enemy!\n";
				}
				else
					cout<<"You hack at your enemy with tremendous strength.\n";
			}
			break;
			case 15://pray
			{
				atk=false;
				cout<<"You clasp your hands together and pray.\n";
				bool pch=roll20(pc.lck*2);
				if( roll20(3) )
				{
					cout<<"You feel an aura of warmth and light from\nthe heavens.\n";
					pc.dex++;
					pc.lck++;
				}
			}
			break;
			case 16://assess
			{
				atk=false;
				cout<<"You study the enemy carefully, assessing its capabilities.\n";
				enemy.showInfo();
			}
			break;
			case 17://reanimate
			{
				atk=false;
				if(pc.area.foe.name != "null" && pc.area.foe.alive==false)
				{
                    pc.area.foe.alive=true;
                    pc.area.foe.hp+=pc.area.foe.lvl*10;
                    pc.area.foe.status = "undead";

                    cout<<"You shed a drop of blood on your enemy's fallen\ncorpse, infusing the body with magical energy by\nmuttering a profane incantation. The monster arises, undead.\n";
				}
				else
					{
					    cout<<"There's nothing to reanimate!\n";
					    pc.mp += pc.casting.cost;
					}
			}
			break;
			case 18://Venom
			{
				atk=true;
				cout<<"You take out a small vial of lethal snake venom\nand pour it on your weapon before attacking.\n";
				if (rollfor(7, 10))
					{
						enemy.status="poisoned";
						cout<<"The venom enters the wound.\nYour enemy turns pale and sickly.\n";
					}
				else
					cout<<"The venom fails to enter the wound.\n";
			}
			break;
			case 19://defend
			{
				atk=false;

				bool shield=checkeq(3, "subt", "shield");
				if (shield==true)
				{
					cout<<"You lift your shield for cover, bracing\nyourself behind it.\n";
					pc.def*=1.5;
				}
				else
					cout<<"You need a shield to do that!\n";
			}
			break;
			case 20://blessing
            {
                pc.lck +=4;

                cout<<"You close your eyes and murmur a sacred blessing.\nYou feel your fortunes change.\n";
            }
			break;
			case 21://ward
            {
                pc.acc.ench = "holy";
                pc.arm.ench = "holy";
                pc.def += 3;

                cout<<"You raise your hands against your opponent's attack,\ncreating a wall of solid light.\n";
            }
			break;
			case 22://burningblade
			{
				atk=false;
				pc.weap.ench="fire";
				pc.weap.enchb+=roll(pc.intl/2);
				cout<<"You pass a hand over your weapon, coating it in\nmagical fire.\n";
			}
			break;
			case 23://aquablade
			{
				atk=false;
				pc.weap.ench="water";
				pc.weap.enchb+=roll(pc.intl/2);
				cout<<"You pass a hand over your weapon, coating it in\nan aura of suspended water.\n";
			}
			break;
			case 24://boulder
			{
                cout<<"At a sharp gesture and a stamp of your foot, a massive chunk of rock\nlaunches itself into the air, and then into your foe.\n";
			}
			break;
			case 25://torrent
			{
                cout<<"The sound of rushing water slowly builds as a giant wave towers over your opponent,\ncrashing down with thunderous force.\n";
			}
			break;
			case 26://gale
			{
			    cout<<"A tearing wind batters your opponent.\n";
			}
			break;
			case 27://ray
			    {
			        cout<<"You call piously to the heavens - a beam of incandescent light pierces the clouds to sear your foe.\n";
			    }
			break;
			case 28://vinewreath
			    {
			        cout<<"You stretch out a hand, causing vines to erupt from the earth and bind your enemy.\n";
			        enemy.dex-=rand()%5+1;
			        if(roll20(4)){ enemy.status="paralyzed"; cout<<"Your foe is paralyzed by tangling vines!\n";}
			    }
			break;
			case 29://crow
			    {
			        cout<<"A piercing cry escapes your throat.\nA bolt of black feathers drops from the sky with raking talons, wounding the enemy.\n";
			    }
			break;
			case 30://reaper
			    {
			        cout<<"The sky suddenly goes black, your souls twin orbs in endless darkness.\n";
			        if(rollfor(pc.intl, 30)){cout<<"You feel your blood freeze as a horrifying shadow passes over you.\nA scream echoes in the darkness; as the world fades back into focus, you see no trace of the enemy.\n";
			        enemy.alive=0;}
			    }
			break;
			case 31://cleave
			    {
			        cout<<"You swing your weapon in a mighty horizontal slash, intending to split your enemy in half.\n";
			        if(roll20(pc.str))
                    {
                        cout<<"You obliterate your foe's weak defense with an overpowering cleave!\n";
                        mdmg=pc.casting.maxd;
                    }
			    }
			break;
			case 32://stealth
			    {
			        cout<<"You erase your breath and vanish into the darkness.\n";
			        int bonus, divisor;
			        string stealth[3] = {"You vanish before the enemy's eyes", "You manage to break the opponent's line of sight", "It doesn't work at all..."};
			        divisor = rollfz(10);

			        if(divisor > 0 && divisor < 5) prln( stealth[0] );
			        else if (divisor > 5) prln( stealth[1] );
			        else prln( stealth[2] );

			        bonus = pc.dex/divisor;
			        pc.dex += bonus; //Increase dex by up to 100%
			    }
			break;
			case 33://sunsear
			    {
			        cout<<"From your gently outstretched hand radiates a blinding light,\nincreasing in intensity until it begins to burn your foe.\n";
			        if(enemy.elem=="dark")
                    {
                        cout<<"Your dark-hearted enemy lets out an anguished cry\nas pure, vibrant light washes over them.\n";
                        mdmg*=2;
                    }
			    }
			break;
			case 34://iceblade
            {
				atk=false;
				pc.weap.ench="ice";
				pc.weap.enchb+=roll(pc.intl/2);
				cout<<"You pass a hand over your weapon, coating it in magical frost.\n";
			}
			break;
			case 35://sparkblade
            {
				atk=false;
				pc.weap.ench="lightning";
				pc.weap.enchb+=roll(pc.intl/2);
				cout<<"You pass a hand over your weapon, imbuing it with crackling energy.\n";
			}
			break;
			case 36://lifedrain
			    {
			        cout<<"Your spell enters the foe's body, streaming back to you in a crimson river.\nYou've stolen its vitality!\n";
			        pc.hp+=mdmg;
			    }
			break;
			case 37://doomshadow
			    {
			        cout<<"A sudden, unnatural stillness presses down upon the\nbattlefield as a monstrous, twisted shadow looms overhead.\n";
			        if(roll20(pc.intl))
                    {
                        cout<<"The pressure crushes your foe to their knees, breaking their spirit.\n";
                        enemy.str--; enemy.dex-=2; enemy.intl--; enemy.lck-=5;
                    }
			    }
			break;
			case 38://grow
			    {
                    cout << "Kneeling in the dirt, you place your palm flat on the ground, closing your eyes.\n\n";

                    switch (mdmg)
                    {
                        case 1 ... 10: {prln("A leafy tendril grows from the ground nearby and slaps the enemy.");} break;
                        case 11 ... 20: {prln("A broad tree limb swings out, striking your foe powerfully.");} break;
                        case 21 ... 30: {prln("Grass and flowers burst forth in a wide radius, joined by a\ngnarled thicket of trees, belaboring your enemy with their branches.");} break;
                        case 31 ... 40: {prln("A great slithering and rumbling begins to shake the earth,\ngrowing louder as if approaching from afar;\n\nA massive forest thick with undergrowth races over the horizon,\ntearing up the ground with its furious approach.\n\nYour foe is swallowed whole by the tangle of bark and branches,\ntheir screams echoing through the forest.");} break;
                    }
			    }
			break;
			case 39://sunmeld
			    {
			        cout<<"Calling upon your link to the sun, you channel\nunimaginable heat, melding your foe's metal equipment.\n";
			        enemy.def-=rand()%mdmg;
			    }
			break;
			case 40://inscribe
			    {
			        cout<<"What rune to inscribe?\n";
			        cin>>pc.inp;
			        pc.casting.elem=pc.inp;
			        mdmg=rand()%pc.intl+1+pc.litlvl;
			        cout<<"You scrawl a quick "<<pc.inp<<" rune into the dirt, unleashing it for "<<mdmg<<" damage.\n";
			    }
			break;
			case 41://kick
			    {
			        cout<<"You lash out with a swift kick.\n";
			        if(rollfor(pc.dex, 20))
                    {
                        mdmg+=2;
                        enemy.status="Staggered";
                        enemy.dex--;
                        cout<<"The blow knocks your enemy off balance!\n";
                    }
			    }
			break;
			case 42://vigor
			    {
			        atk=false;
			        cout<<"You belt out a vigorous song, energizing your party.\n";
			        pc.hp++; pc.str++;
			    }
			break;
			case 43://charm
			    {
			        atk=false;
			        cout<<"You murmur a charming tune, lowering your foe's aggression.\n";
			        enemy.atk--; enemy.mp--;
			    }
			break;
			case 44://swarm
			    {
			        cout<<"A far-off droning buzz becomes a deafening roar.\nThousands of stinging insects envelope your foe,\ncreating a vortex of pain and poison before buzzing away.\n";
			        if(rollfor(pc.intl, 20))
                    {
                        enemy.status="poisoned";
                        cout<<"\nThe innumerable stings have done their work; the enemy reels with deadly poisoning!\n";
                    }
			    }
			break;
			case 46://bear
			    {
			        cout<<"Responding to your guttural bellow, a massive brown bear charges your opponent,\nshaking the ground with its steps and delivering a bone-shattering swipe.\n";
                    if (pc.comp.name == "empty") {
                        pc.comp.name == "Bear";
                        pc.comp.lvl = roll(pc.intl);
                        pc.comp.atk = pc.comp.lvl + 1;
                        pc.comp.def = pc.comp.lvl + 1;
                        pc.comp.hpmax = pc.comp.lvl * 5;
                        pc.comp.hp = pc.comp.hpmax;
                    }
			    }
			break;
			case 47://blessedblade
			    {
			        cout<<"You hold your blade upright with a silent prayer before striking out.\n";
			        pc.atk+=pc.intl/4;
			        pc.weap.ench="holy";
			        pc.weap.enchb+=pc.intl/4;
			    }
			break;
			case 48://holyshield
			    {
			        atk=false;
			        cout<<"You cast aside your fear for faith.\nYour belief strengthens your defense.\n";
			        pc.def+=pc.intl/4;
			        pc.arm.ench="holy";
			        pc.arm.enchb+=pc.intl/4;
			    }
			break;
			case 49://presence
			    {
			        atk = false;
			        cout << "You allow an immense otherworldly presence to fill your soul.\n With an explosive surge of power, you are surrounded by a nimbus of flickering light;\nyour hair flutters and the ground underneath you begins to shake\nas rocks and earth are blown aside by your aura.\n";
                    pc.hp += 20;

                    pc.atk += rollcap( pc.lck/ 2, pc.lck, 10);
                    pc.def += rollcap( pc.lck/ 2, pc.lck, 10);
                    pc.str += rollcap( pc.lck/ 2, pc.lck, 10);
                    pc.dex += rollcap( pc.lck/ 2, pc.lck, 10);
                    pc.intl += rollcap( pc.lck/ 2, pc.lck, 10);
			    }
			break;
			case 50://banish
			    {
			        cout<<"With a powerful, alien shout, you release a blast of incandescent light.\n";
			        if(enemy.elem=="dark")
                    {
                        cout<<"Your dark-hearted foe crumbles into ash, blown away in the blast.\n";
                        enemy.alive=false;
                    }
			    }
			break;
			case 51://vitalstrike
			    {
			        cout<<"Your keen eye spots a weakness; without hesitation you lash out\nwith the speed of a striking snake.\n";
			        if(rollfor((pc.lck*3), 100))
                    {
                        mdmg*=3;
                        cout<<"Your weapon finds its mark, vitally wounding the foe!\n";
                    }
			    }
			break;
			case 52://shadowstep
			    {
			        atk=false;
			        cout<<"With a deep breath, you vanish into the shadows, pushing your speed\nto its utmost limits as you flicker around your foe,\nbarely visible even as an afterimage.\n";
			        pc.dex+=pc.intl;
			    }
			break;
			case 53://skyfall
			    {
			        cout<<"You raise a hand and bring it swiftly down.\nIn the blink of an eye, heavy black clouds cover the horizon.\nThousands of thundering tendrils of electricity slam into the earth in a devastating armageddon.\n";

			    }
			break;
			case 54://starfall
			    {
			        cout<<"You step back several paces, raising a hand overhead.\nIn your palm, in the wink of an eye, a massive star forms, sucking greedily at the atmosphere\nand tearing out trees by their roots with its intense gravity.\nWith a flick of your wrist, the dwarf star collides with your opponent in a blast that can be seen for miles.\n";
			    }
			break;
			case 55://infatuate
			    {
			        cout<<"Your enemy takes a step back, thrown off-guard and intrigued by your sudden glamour.\n";
			        if(roll20( (pc.intl + pc.lck) - enemy.intl) )
                    {
                        cout<<"The light of battle has gone out of their eyes, replaced by urgent desire.\n";
                        enemy.atk/=2; enemy.def/=2; enemy.lck++;
                        enemy.status = "paralyzed";
                    }
			    }
			break;
			case 56://skewer
			    {
			        mdmg+=enemy.def;
			        cout<<"Coiling your arm back to your chest, you thrust forward, piercing your enemy's armor.\n";
			    }
			break;
			case 57://riposte
			    {

			    }
			break;
			case 58://empower
			    {

			    }
			break;
			case 59://pacify
			break;
			case 60://steal
			break;
			case 61://stab
			break;
			case 62://diversion
			    {
			        atk=false;
			        cout<<"You toss a flurry of augury cards from your sleeves, distracting the enemy.\n";
			        enemy.intl-=2; enemy.dex-=2; enemy.lck-=2;
			    }
			break;
			case 63://cardtrick
			    {
			        /*Add in "augury deck", required for cardtrick*/

			        int val=rand()%3+1;
			        int suit=rand()%3+1;
			        int face=rand()%8;
			        string suits[4]={" ", "leaves", "blades", "face"};
			        string faces[8]={"drifting feathers", "stone spire", "frozen spire", "flame", "blue flame", "sunrise", "sunset", "soul"};
			        cout<<"You slip an augury card into your hand.\n";
			        cout<<"It's the ";
			        if (suit<3)
                    {
                        cout<<val<<" of "<<suits[suit]<<".\n";
                        val*=suit; mdmg+=val;
                    }
                    else
                    {
                        cout<<"'"<<faces[face]<<"' card.\n";
                        switch(face)
                        {
                        case 0: {cout<<"You feel a little lighter.\n"; pc.dex+=3;} break;
                        case 1: {cout<<"You feel a little bolder.\n"; pc.str+=3;} break;
                        case 2: {cout<<"Your mind feels a little sharper.\n"; pc.intl+=3;} break;
                        case 3: {cout<<"Your hand stings with a sudden scorching pain.\nYou take 5 damage.\n"; pc.hp-=5; mdmg+=5;} break;
                        case 4: {cout<<"You feel a sudden chill as your energy drains away.\nYou take 5 mana damage.\n"; pc.mp-=5; enemy.mp-=5;} break;
                        case 5: {cout<<"You feel a dawning of hope as the sun rises.\n"; timen=1; pc.lck++;} break;
                        case 6: {cout<<"You feel a misty pang of loss as the sun sets.\n"; timen=7; pc.lck--;} break;
                        case 7: {cout<<"Your eyes glow as your energy exceeds its natural bounds.\n"; pc.mp+=pc.mpmax; mdmg+=roll(10);} break;
                        }
                    }
                    cout<<"You magically harden the edge and slash!\n";
                }
			break;
			case 64://vanish
			    {
			        atk=false;
			        cout<<"You bend the light around you, appearing translucent.\n";
			        pc.dex+=pc.intl;
			    }
			break;
			case 65://mug
			    {
			        cout<<"You brutally slug the enemy, catching a few coins that fall loose.\n";
			        pc.gp+=rand()%pc.lck+2;
			    }
			break;
			case 66://sense
			    {
			        atk=false;
			        cout<<"It is "<<curs.weath.name<<".\n";
			        cout<<"Your enemy appears to have innate "<<enemy.elem<<" energy,\nand may be susceptible to "<<enemy.weak<<" attacks.\n";
			    }
			break;
			case 67://hide
			    {
			        cout<<"You attempt to break your opponent's line of sight.\n";
			        if(rollfor(pc.dex, 20))
                    {
                        cout<<"You duck swiftly out of sight.\n";
                        pc.dex++;
                    }
                    else cout<<"Your enemy's eye is too quick!\n";
			    }
			break;
			case 68://meditate
			    {
			        if (pc.lvl < 10) cout<<"You're not seasoned enough to calm your mind in the midst of battle!\n";
			        else{
                        cout << "You shut out the battlefield, calming your mind and letting go your thoughts.\n";
                        pc.mp += 2 + roll(pc.intl);
			        }
			    }
			break;
			case 69: //poultice
                {
                    atk = false;
                    cout << "You affix a medicinal poultice of leaves and mud to your wounds.\n";
                    if (pc.status == "poisoned") {
                        cout << "The poultice draws out the poison in your veins.\n";
                        pc.status = "none";
                    }
                }
            break;
            case 70: //Herblore
                {
                    cout << "You see no reason to do that in combat!\n";
                }
            break;
			case 99: //Transmute
            {
                cout<<"Reaching out with your mind, you attune the foe's elemental energies;\n";
                string elem[6]={"fire", "water", "earth", "wind", "lightning", "ice"};
                cout<<"What will you change their weakness to?\n";
                for(int x=0;x<6;x++) cout<<" -"<<elem[x]<<"\n";
                enemy.weak = pc.getInps();
                cout<<"What will you change their strength to?\n";
                enemy.elem = pc.getInps();
                cout<<"With sudden, intense focus, you shift your enemy's attunement.\n";
            }
		}

		if(admin) cout << "\nDebug: passed switch/case\n";

		//if damage greater than max, set to max (except for inscribe, for some reason)
		if(mdmg>pc.casting.maxd&&pc.casting.name!="inscribe")
				mdmg=pc.casting.maxd;

        if(admin) cout << "\nDebug: passed damage maximum\n";

        if(pc.casting.elem=="physical"&&pc.weap.ench!="none") pc.casting.elem=pc.weap.ench;

        if(admin) cout << "\nDebug: enchantment passed from weapon to physical attack\n";

		//Elemental damage bonus (season/weather)
		if (atk){
            if (pc.casting.elem == curs.elem1) mdmg += 2;
            else if (pc.casting.elem == curs.elem2) mdmg += 1;

            if(pc.casting.elem == curs.weath.elem) mdmg += 2;
		}

		if(admin) cout << "\nDebug: passed elemental damage bonus\n";

		//Elemental effects
		if(atk){
            if(pc.casting.elem=="fire") {}
            else if(pc.casting.elem=="water")
            {
                if(pc.area.fire)
                {
                    cout<<"Your water spell douses the nearby campfire.\n";
                    pc.area.fire=false;
                }
                if( roll20(1) )
                {
                    cout<<"Your enemy inhales water, choking for an extra "<<(mdmg/100)+1<<" damage!\n";
                    mdmg+=(mdmg/10)+1;
                }
            }
            else if(pc.casting.elem=="ice")
            {
                enemy.dex-=mdmg/5;
                cout<<"Your frost spell has slowed the enemy.\n";
            }
            else if(pc.casting.elem=="electric")
            {
                int paralyze_chance = (pc.intl + pc.lck) - enemy.intl;
                if( rollfor(paralyze_chance, 33) )
                {
                    enemy.status="paralyzed";
                    cout<<"Your electric attack paralyzed your enemy!\n";
                }
            }
            else if(pc.casting.elem=="wind")
            {
                if( rollfor(pc.intl, 100) )
                {
                    cout<<"Your enemy staggers back in strong winds.\n";
                    enemy.status="stagger";
                }
            }
            else if(pc.casting.elem=="earth")
            {
                int tdamb = 0;
                if (pc.area.is_outside && pc.area.name != "Tundra") {
                        if (pc.area.wood) tdamb = roll(20);
                        if (pc.area.mine) tdamb = roll(10);
                        else tdamb = roll(6);
                }
                if(tdamb>0)
                {
                    cout<<"The wilds around you add power to your earth spell!\n";
                    mdmg+=tdamb;
                }
            }
		}

		if(admin) cout << "\nDebug: passed elemental effect\n";

		if(pc.casting.elem=="heal")
		{
			int heal=rand()%(pc.casting.dmg+pc.intl)+1;
			if(heal>pc.casting.maxd)
				heal=pc.casting.maxd;
			pc.hp+=heal;
			cout<<"You regain "<<heal<<"HP.\n";
		}

		if(admin) cout << "\nDebug: passed heal step\n";

        if(mdmg>0){
            if(enemy.weak==pc.casting.elem) mdmg*=2;
            if(enemy.elem==pc.casting.elem) if(mdmg>0) mdmg/=2;}

        if(admin) cout << "\nDebug: passed elemental strength/weakness damage boost\n";


        /* CASTING BUG HERE
        -------------------------------+
            Spells failing during casting
            caught by debug message above ^

            - Added catch for zero-enchb gear
        */


        //Alter damage based on gear enchantments
        if(pc.weap.ench==pc.casting.elem&&pc.weap.enchb>0) mdmg+=rand()%pc.weap.enchb;
        if(pc.acc.ench==pc.casting.elem&&pc.acc.enchb>0) mdmg+=rand()%pc.acc.enchb;

        if(admin) cout << "\nDebug: passed gear enchantment bonuses\n";


        //Damage Step
		if(pc.casting.elem!="heal"&&atk==true)
		{
		    string s_dmg_scale[3] = {"barely any", "moderate", "considerable"};
		    int n_dmg_scale = 0;

			enemy.hp-=mdmg;
            if (mdmg <= 2) n_dmg_scale = 0;
			else if (mdmg >= pc.casting.maxd-3) n_dmg_scale = 2;
			else if (mdmg >= pc.casting.maxd/2) n_dmg_scale = 1;

            //Damage output
			cout<<"\nThe enemy takes ";
			if( game_settings.combat_display == "text" ) cout << s_dmg_scale[n_dmg_scale];
            else cout << mdmg;

            cout << " damage.";
			enemy.showHealth();
		}

		//Show player's energy status
		pc.showMana();

    cout<<"\n";

	return enemy;
}

void noncomcast(string name, int id)
{
	for(int x=0;x<70;x++)
		if(splist[x].name == name||splist[x].id==id)
			{pc.casting=splist[x];}
    if(name=="?"){pc.casting.cost=0; pc.casting.unlock=true;}

	if(pc.mp>=pc.casting.cost)
	{
		if(pc.casting.unlock==true)
		{
			pc.mp-=pc.casting.cost;
			cout<<"You cast "<<pc.casting.name<<".\n";
			switch(pc.casting.id)
			{
				case 0:
				{
					cout<<"Light a fire with char?(y/n)\n";
					cin>>pc.inp;
					if(pc.inp=="y")
					{
						int x=searchinv(49);
						if(x!=0)
						{
							pc.area.fire=true;
							pc.carrywt-=pc.inv[x].wt;
							pc.inv[x]=eq[0];
							cout<<"You channel heat through your hand to light a fire.\n";
						}
						else
							cout<<"You consider lighting your surroundings on\nfire but wisely decide against it.\n";
					}
				}
				break;
				case 1:
					cout<<"You shiver as the temperature drops.\n";
				break;
				case 3:
				{
					cout<<"A stiff wind blows through the area.\n";
					if(pc.area.fire==true)
					{
						cout<<"Your fire thrashes desperately before being\nsnuffed out.\n";
						pc.area.fire=false;
					}
				}
				break;
				case 2:
					cout<<"Small bolts of bright electricity arc and snap\nthrough the air.\n";
				break;
				case 4:
					cout<<"A sharp growth of stone juts out of the earth\nbefore you, then sinks back down.\n";
				break;
				case 5:
					cout<<"You thrust your weapon forward in a tightly\ncontrolled stabbing motion.\n";
				break;
				case 6:
				{
					int hpg=rand()%10+pc.intl;
					cout<<"You bestow healing magic upon your wounds.\n"<<hpg<<" HP restored.\n";
					pc.hp+=hpg;
				}
				break;
				case 7:
				{
				    //Add a mechanic for throwing dye at things
					cout<<"Throw which item? (inv # or 0 to throw weapon)\n";
					int num;
					num=check_int();;
					if(num==0&&pc.weap.name!="empty")
					{
						cout<<"You throw your "<<pc.weap.name<<" at the ground.\n";
						pc.carrywt-= pc.weap.wt;
						pc.area.loot=pc.weap;
						pc.weap=eq[0];
					}
					else if(pc.inv[num].name!="empty")
					{
						cout<<"You throw the "<<pc.inv[num].name<<" at the ground.\n";
						pc.carrywt-= pc.inv[num].wt;
						pc.area.loot=pc.inv[num];
						pc.inv[num]=eq[0];
					}
					else
						cout<<"Nothing to throw!\n";
				}
				break;
				case 8://crush
					cout<<"You swing your weapon in a heavy, overhanded arc.\n";
				break;
				case 9://poison
					cout<<"You crush some poisonous leaves along the length\nof your blade; but after a couple minutes\nin the open air, the weak poison goes stale.\n";
				break;
				case 10://flush
				{
					cout<<"Concentrating deeply, eyes closed, you draw\nwater into your body, flushing out toxins and impurities.\n";
					pc.status="none";
				}
				break;
				case 11://fireball
					cout<<"You summon an intense ball of fire over your palm.\nYou then release the spell and it dissolves,\nleaving you blinking in the bright afterimage.\n";
				break;
				case 12://mist
				{
					cout<<"You summon a thick fog.\n";
					pc.area.desc+="There is a dense, swirling mist blanketing the area.\n";
				}
				break;
				case 13://drench
				{
					cout<<"You draw water from the air to form a sphere.\nIt hangs for a moment before splashing to the ground.\n";
					if(pc.area.fire==true)
					{
						cout<<"Your fire sizzles and goes out, damp wood smoldering.\n";
						pc.area.fire=false;
					}
				}
				break;
				case 14://hack
					cout<<"You flex your arms and swing your weapon\nmightily, burying the edge into the dirt.\n";
				break;
				case 15://pray
				{
					cout<<"Closing your eyes, you press your hands together\nand solemnly pray to the heavens for healing\nand blessing.\n";
					int hpg=rand()%15+5;
					if(pc.area.name == "Temple of Light")
                    {
                        hpg*=2;
                        if( rollfor(pc.karma, 862) )
                        {
                            prln("You feel as though something has heard your prayer.\nAn aura of peace and well-being infuses your spirit.");
                            pc.lck++;
                        }
                    }
					pc.hp+=hpg;
					cout<<"You regain "<<hpg<<" HP.\n";
				}
				break;
				case 16://assess
				{
				    pc.showInv();
					cout<<"\nEnter an inventory number, or 11 - 13 for weapon, armor, accessory:\n";
					int num = pc.getInpn();
					if(num>0&&num<11)
					{
						cout<<"\nYou assess the item.\n";
						pc.inv[num].showData("assess");
					}
					else if (num > 10 && num < 14)
                    {
                        prln("You assess the item.");
                        switch(num)
                        {
                            case 11: {pc.weap.showData("assess");} break;
                            case 12: {pc.arm.showData("assess");} break;
                            case 13: {pc.acc.showData("assess");} break;
                        }
                    }
					else cout<<"You can't assess that!\n";
				}
				break;
			case 17://reanimate
			{
				if(pc.area.foe.alive==false)
				{
				pc.area.foe.alive=true;
				pc.area.foe.hp+=pc.area.foe.lvl*10;
				cout<<"You shed a drop of blood on your enemy's fallen\ncorpse, infusing the body with magical energy by\nmuttering a profane incantation.\nThe monster arises, mindless and undead.\n";
				}
				else
					cout<<"You've wasted your energy; there's nothing to reanimate!\n";
			}
			break;
			case 18://venom
			{
				cout<<"You poison your weapon. The single-use poison dries rapidly in the open air,\nlosing its potency.\n";
			}
			break;
			case 19://defend
			{
				cout<<"You raise your shield in a swift motion, then lower it again.\n";
			}
			break;
			case 20://blessing
			{
				cout<<"You say a simple prayer.\n";
			}
			break;
			case 21://ward
			{
				cout<<"You lift your hand, warding off an attack in your mind's eye.\n";
			}
			break;
			case 22://burningblade
			{
				cout<<"You coat your weapon in magical fire. It quickly dissipates.\n";
			}
			break;
			case 23://aquablade
			{
				cout<<"You coat your weapon in magical water. It quickly dissipates.\n";
			}
			break;
			case 24://hailstorm
			{
				cout<<"You focus your power and summon a thunderous hailstorm.\nAs you hastily shield your head from the pelting hailstones,\nyou wonder what purpose you had for this.\n";
				curs.weath=cweath[1];
			}
			break;
			case 25://torrent
			{
				cout<<"A roaring tide of water sweeps the area clear.\nAs the wave subsides you slacken your grip on whatever anchor \nyou could find and stand sodden and dripping.n";
				if(pc.area.fire)
				{
					cout<<"Your campfire goes out.\n";
					pc.area.fire=false;
				}
				pc.area.loot=eq[0];
				pc.area.loc_flora=plant[0];
				pc.area.loc_fauna=anim[0];
			}
			break;
			case 26://gale
			{
				cout<<"A tearing wind tugs at your clothing.\n";
			}
			break;
			case 27://ray
			{
				cout<<"The clouds part slightly, dazzling you with a solitary sunbeam.\n";
			}
			break;
			case 28://vinewreath
			{
				cout<<"You close your eyes and focus, feeling the hum of the earth.\nIn moments the surrounding area is overgrown with vines and flowers.\n";
			}
			break;
			case 29://crow
			{
				cout<<"You squint up at the sky and release an avian shriek.\nAfter a moment, a ragged old crow flutters down to land on your shoulder.\nIt cocks its head, caws back, and flaps up into the trees where it waits, watching.\n";
                pc.area.loc_fauna=anim[19];
			}
			break;
			case 30://reaper
			{
				cout<<"You feel a deep sense of dread and an unnatural chill. You quickly end the spell, drawing your mana back from the fade.\n";
				pc.mp+=pc.casting.cost;
			}
			break;
			case 31://cleave
			{
				cout<<"Holding your weapon high above your head, you bury the business end in the earth.\nIt takes you several seconds of tugging to retrieve it.\n";
			}
			break;
			case 32://stealth
			{
				cout<<"You erase your breath and vanish into the shadows.\nToo bad nobody's around to not see you.\n";
			}
			break;
			case 33://sunsear
			{
				cout<<"You reach your arms into the sky, imploring;\na sudden and intense burst of light scorches the ground before you.\n";
			}
			break;
			case 34://iceblade
			{
				cout<<"You coat your weapon in magical frost.\nIt quickly melts.\n";
			}
			break;
			case 35://sparkblade
			{
				cout<<"You channel crackling energy through your weapon.\nIt quickly snaps out into the air and disapears.\n";
			}
			break;
			case 36://lifedrain
			{
				cout<<"You place a hand flat on the earth, leeching life force from your surroundings.\nA little patch of dead grass radiates outward from you.\n";
				int lfg=rand()%(pc.intl*2)+1;
				pc.hp+=lfg;
				cout<<"You gain "<<lfg<<" hp\n";
			}
			break;
			case 37://doomshadow
			{
				cout<<"A wicked, winged shadow passes overhead.\n";
			}
			break;
			case 38://grow
			{
				cout<<"You close your eyes, feeling the flow of the earth.\nIn mere moments the area is overgrown with vegetation and flowers.\n";
				if(pc.area.name=="Farmland")
				{
					pc.area.crops=true;
				}

				//Roll for flora until something grows, checking
				//to ensure that something can
				while (pc.area.loc_flora.name == "null" && pc.area.flora_types.size() != 0) pc.area.load_flora();

			}
			break;
			case 39://sunmeld
			    {
			        cout << "You channel the energy of the sun, enough to meld metal...\n\n";
			        smith();
                }
			break;
			case 40://inscribe
            		{
                    if(!pc.chis){ cout<<"You need a chisel to inscribe an item.\n"; return;}
                	else
                	{
                        pc.showInv();
                        cout<<"What item do you want to inscribe?\n";
                        cout<<"(inv# 1-10)\n";
                        int x = pc.getInpn();
                        cout<<"What runes would you like to inscribe onto the item?\n";
                        cout<<"('fire' for a fire attunement, etc)\n";
                        pc.inp = pc.getInps();
                        pc.inv[x].ench=pc.inp;
                        cout<<"\nTaking your chisel in hand, you firmly etch\nthe runes for '"<<pc.inp<<"' into the item's surface.\n";
                	}
			}
			break;
			case 44: //Swarm
            {
                cout << "You emit a strange buzzing sound, imitating the hum of insect wings.\n\nA curious bee arrives in seconds, hovering at a distance.\n";
                pc.area.loc_fauna = anim[16];
            }
            break;
            case 45: //Raise
            {
                //Will resurrect a dead enemy to serve as either
                //a companion, a follower (the lost), or an enemy

                //Check for dead enemy
                //Uninitialized (blank) enemy will always be at lvl 0

                if (!pc.area.foe.alive && pc.area.foe.lvl > 0)
                {
                    cout << "Closing your eyes, you feel out the fibrous\nmuscles and tendons holding the corpse together.\n";
                    cout << "\nYou find the creature's wounds in your mind,\nknitting the flesh and bone back together.\n";
                    cout << "The creature arises, confused but grateful.\n\n";

                    if (pc.comp.name == "null" || pc.comp.name == "empty")
                    {
                        enem e = pc.area.foe;
                        character c;

                        c.name = e.name;
                        c.clas = "undead";
                        c.lvl = e.lvl;
                        c.hp = c.lvl * 10;
                        c.mp = c.lvl * 10;
                        c.iab = e.ab.id;
                        c.atk = e.atk;
                        c.def = e.def;
                        c.str = e.str;
                        c.dex = e.dex;
                        c.intl = e.intl;
                        c.lck = e.lck;

                        for (int i = 0; i < 5; i++) c.idle[i] = e.idle;

                        pc.comp = c;

                        cout << "Name your new monster companion? (Y / N)\n";
                        if ( inpAffirm( pc.getInps() ) ) pc.comp.name = pc.getInps();
                        else pc.comp.name = pc.name + "'s " + pc.comp.name;
                    }
                    else if (!m_data.lost_in_party)
                    {
                        side_char sc;
                        string morf[2] = {"man", "woman"};

                        enem e = pc.area.foe;

                        sc.create("undead", "", e.name, morf[ rollfz(2) ] );
                        sc.c_type = "enemy"; //Handle for interactions
                        sc.initialize(false); //Sets pronouns
                        for (int i = 0; i < 5; i++) sc.idle[i] = e.idle; //SC idles = enemy idle

                        //Change gender noun to enemy name
                        //to avoid addressing different species incorrectly
                        sc.mf = e.name;
                        sc.n = e.name;

                        //Sets enemy-derived side character as the Lost
                        lost = sc; m_data.lost_in_party = true;

                        //Sets a destination where enemy character can be dropped off for XP and GP
                        int x, y;
                        do {
                            x = roll(10);
                            y = roll(10);
                        } while (loc[x][y].name != "River");
                        m_data.lostx = x;
                        m_data.losty = y;

                        prln("The " + lost.mf + " begins to follow you.\nWho knows for how long?");
                    }
                    else {
                            pc.area.foe.alive = true;
                            pc.area.foe.hp = 50;
                    }
                }
                else
                {
                    prln("You don't see any corpses to reanimate.");
                }
            }
            break;
			case 46://bear
                {
                    cout<<"You suck in a deep breath and utter a brazen roar that makes the ground shake.\nWithin seconds, a surly-looking brown bear lopes into the clearing.\nYou share a meaningful look for a few seconds before the bear turns to a nearby bush.\n";
                    pc.area.loc_fauna=anim[5];
                }
                break;
			case 63://cardtrick
                	{pc.mp += pc.casting.cost;
                    int val=rand()%3+1;
			        int suit=rand()%3+1;
			        int face=rand()%8;
			        string suits[4]={" ", "leaves", "blades", "face"};
			        string faces[8]={"drifting feathers", "stone spire", "frozen spire", "flame", "blue flame", "sunrise", "sunset", "soul"};
			        cout<<"You pull an augury card at random from your deck.\n";
			        cout<<"It's the ";
			        if (suit<3) cout<<val<<" of "<<suits[suit]<<".\n";
                    		else cout<<"'"<<faces[face]<<"' card.\n";
                	}//end of 'cardtrick'
			break;
			case 64://
                {

                }
                break;
            case 65://
                {

                }
                break;
            case 66://sense
                {
                    int fauna_size=pc.area.fauna_types.size();
                    int flora_size=pc.area.flora_types.size();
                    string plant;
                    if(flora_size>0) plant = pc.area.flora_types[rand()%flora_size].name;
                    else plant = "grass";
                    string anim;
                    if(fauna_size>0) anim = pc.area.fauna_types[rand()%fauna_size].name;
                    else anim = "bugs";
                    string elem;
                    if(curs.weath.name=="Raining") {elem="water";}
                    if(curs.weath.name=="Stormy") {elem="water and lightning";}
                    if(curs.weath.name=="Sunny") {elem="fire";}
                    if(curs.weath.name=="Clear") {elem="wind and earth";}
                    if(curs.weath.name=="Snowing") {elem="water and ice";}
                    if(curs.weath.name=="Windy") {elem="wind";}
                    cout<<"\nYou tune your senses towards the natural world.\n\n";
                    cout<<"This area appears to be capable of sustaining "<<plant<<" and "<<anim<<".\n";
                    cout<<"It is "<<curs.weath.name<<"; "<<elem<<" spells should be stronger for a little while.\n";
                    cout<<"You pick up the faint scent of danger -\n there could be a "<<elist[pc.area.enemies[rand()%3]].name<<" lurking somewhere nearby.\n";
                }
                break;
            case 68: //meditate
                {
                    cout << "You close your eyes, calming your mind and letting go your thoughts.\n";
                    timen++;
                    pc.mp += 2 + roll(pc.intl);
                }
            break;
			default:
			{
				cout<<"You see no reason to use that ability outside of combat.\n";
			}
			break;

			}//end of switch/case
		}
		else
			cout<<"You don't know that spell yet!\n";
	}
	else
		cout<<"Not enough mana\n";
}//end of noncomcast

enem ecast(enem enemy)
{
	int mdmg;
	bool atk=true;
		enemy.mp-=enemy.ab.cost;
		cout<<"The "<<enemy.name<<" uses "<<enemy.ab.name<<"!\n";

switch(enemy.ab.id)
		{
			case 5://pierce
			{
				mdmg+=pc.def;
				cout<<"Its precise attack pierces through a gap\nin your defenses.\n";
			}
			break;
			case 6: atk=false; break;
			case 8://crush
			{
				pc.def-=1;
				pc.dex-=1;
				cout<<"Its crushing blow damages your\narmor and limits your movement!\n";
			}
			break;
			case 9://poison
			{
				cout<<"It quickly poisons its weapon and attacks;\n";
				bool psch=roll20(enemy.dex);
				if(psch==true)
				{
					cout<<"You've been poisoned!";
					pc.status="poisoned";
				}
				else
					cout<<"The poison fails to enter the wound.\n";
			}
			break;
			case 10://flush
			{
				atk=false;
				enemy.status="none";
				cout<<"It flushes the impurities from its system,/nrestoring its status.\n";
			}
			break;
			case 12://mist
			{
				atk=false;
				cout<<"It summons a swirling cloud of mist,\ncloaking its movements and lowering\nyour accuracy.\n";
				enemy.dex+=2;
				pc.dex-=2;
			}
			break;
			case 13://drench
			{
				cout<<"The enemy hits you with a forceful spray of water.\n";
				bool ch=roll20(2);
				if(ch==true)
				{
					cout<<"You choke and splutter, taking an extra 1 damage.\n";
					mdmg+=1;
				}
			}
			break;
			case 14://hack
			{
				int critc=rand()%30+1;
				if(enemy.lck>=critc)
				{
					mdmg*=3;
					cout<<"Critical hit!\nIt hacks off a limb, severely damaging you!\n";
				}
				else
					cout<<"It hacks at you with tremendous strength.\n";
			}
			break;
			case 15://pray
			{
				atk=false;
				cout<<"It clasps its hands together and prays.\n";
				bool pch=roll20(4);
				if(pch==true)
				{
					cout<<"It is surrounded by an aura of warmth and light\nfrom the heavens.\n";
					enemy.dex++;
					enemy.lck++;
				}
			}
			break;
			case 19: //Defend
                {
                    cout << "\nThe " << enemy.name << " takes a defensive stance, \nreadying itself to ward off your attacks!\n";
                    enemy.def++;
                }
            break;
			case 27:
			break;
			case 28:
			break;
			case 29:
			break;
			case 30:
			break;
			case 31:
			break;
			case 32:
			break;
			case 33:
			break;
			case 34:
			break;
			case 35:
			break;
			case 36:
			break;
			case 37:
			break;
			case 38:
			break;
			case 39:
			break;
			case 40:
			break;

			case 47: //blessedblade
			    {
			        cout << "\nThe " << enemy.name << " holds its blade aloft, eyes closed.\n";
			        cout << "A gleam of inspiring light flashes along the edge.\n";

			        enemy.atk += enemy.lck;
			        enemy.elem = "light";
			    }
            break;
			case 48: //holyshield
			    {
			        cout << "The " << enemy.name << " raises its shield with a heartfelt prayer.\n";
			        cout << "Its defense glows with an alabaster aura.\n";

			        enemy.def += enemy.lck;
			        enemy.elem = "light";
			    }
            break;
            case 71: //acid
            {
                cout << "\nThe " << enemy.name << " spits a venomous glob of acid at you!\n";

                if ( rollfor(1, 2) ) {
                    prln("Your armor starts to sizzle as the acid eats its way through!");
                    pc.def -= roll(pc.def/2);
                }
                if (rollfor(1, 5) ) {
                    prln("Your veins begin to burn...the caustic acid swims through your bloodstream!");
                    pc.status = "poisoned";
                }
            }
			break;
		}

		if(enemy.ab.elem=="fire")
		{
            if ( (pc.weap.mat == "wooden" || pc.weap.type == "bow") && rollfor(enemy.intl, 100) ){
                prln("Your weapon catches alight! You drop it hastily, watching it burn in dismay.");
                pc.unequip(1, "destroy");
            }
		}
		else if(enemy.ab.elem=="ice")
		{
				enemy.dex-=mdmg/5;
				cout<<"The enemy's' frost spell has slowed you down.\n";
		}
		else if(enemy.ab.elem=="electric")
		{
			int paralyze_chance = (enemy.intl + enemy.lck) - pc.intl;
			if( roll20(paralyze_chance) )
			{
				pc.status="paralyzed";
				cout<<"The enemy's electric attack paralyzed you!\n";
			}
		}
		else if(enemy.ab.elem=="wind")
		{
			if( roll20(1) )
			{
				cout<<"You stagger back in strong winds.\n";
				pc.status="stagger";
			}
		}
		else if(enemy.ab.elem=="earth")
		{
			cout<<"The enemy's' earth spell breaks through your armor!\n";
			mdmg+=(pc.def/2);
		}

	if(enemy.ab.elem=="heal")
	{
		enemy.hp+=rand()%enemy.ab.dmg+enemy.intl;
		cout<<"The enemy heals itself. ";
		cout<<"Enemy HP: "<<enemy.hp<<"\n";
	}
	if(atk==true)
	{
		if(enemy.ab.elem=="sword" || enemy.ab.elem == "physical")
			mdmg=rand()%(enemy.ab.maxd+enemy.str)+1;
		else
			mdmg=rand()%(enemy.ab.maxd+enemy.intl)+1;

		pc.hp-=mdmg;
		cout<<"\nYou take "<<mdmg<<" damage!\n";
		pc.showHealth();
	}
	return enemy;
}

void hpotion()
{
	if(pc.hpot>0)
	{
		int hpg=rand()%10+5;
		hpg+=1.5*pc.lvl;
		pc.hpot--;
		pc.hp+=hpg;
		cout<<"You drink a health potion and regain "<<hpg<<" HP.\n";
		if(pc.hp>pc.hpmax)
			pc.hp=pc.hpmax;
	}
	else cout<<"You're out of health potions!\n";
}

void mpotion()
{
	if(pc.mpot>0)
	{
		int mpg=rand()%10+5;
		mpg+=pc.intl;
		pc.mpot--;
		pc.mp+=mpg;
		cout<<"You drink a mana potion and regain "<<mpg<<" MP.\n";
		if(pc.mp>pc.mpmax)
			pc.mp=pc.mpmax;
	}
	else cout<<"You're out of mana potions!\n";
}

void apotion()
{
	if(pc.apot>0)
	{
		cout<<"You drink an antidote. It heals your afflictions.\n";
		if(pc.status=="poisoned"||pc.status=="paralyzed") pc.status="OK";
		pc.apot--;
	}
	else
		cout<<"You're out of antidote potions!\n";
}

enem compFight(enem enemy)
{
    int maxdmg=(pc.comp.atk+pc.comp.str)-enemy.def;
    if(maxdmg<1)maxdmg=1;
    int dmg=rand()%maxdmg+1;

    //[LEGACY] companion combat AI -
    //chance to use attack starts at 50/50
    int ac=10;//attack chance
    if(pc.comp.ai=="aggressive") {ac=15;}//increases if the character is "cunning", decreases if "aggressive"
    if(pc.comp.ai=="cunning") {ac=5;}
    if(pc.comp.mp<splist[pc.comp.iab].cost) {ac=20;}
    if((pc.comp.hp<=(pc.comp.hpmax/3)) && splist[pc.comp.iab].elem=="heal") {ac=20;}//becomes 100% if character is low health and has a healing skill
    if((pc.hp<=(pc.hpmax/3)) && splist[pc.comp.iab].elem=="heal") {ac=20;}//or if player is injured and character has a healing skill

    //Companion combat AI (chance to attack/cast)
    bool attack=pc.comp.chooseAttack(enemy.weak, enemy.def);
    if(attack){
        if(!rollfor(enemy.dex, 100))
        {
            enemy.hp-=dmg;
            cout<<pc.comp.name<<" attacks for "<<dmg<<" damage.\n Enemy HP: "<<enemy.hp<<"\n";
        }
        else cout<<pc.comp.name<<" tries to attack but misses completely.\n";
    }//switch attack or spell
    else
    {
        cout<<"\n";
        special casting=splist[pc.comp.iab];
        pc.comp.mp-=casting.cost;
        int mdmg=rand()%casting.maxd+casting.dmg;
        //Elemental bonus damage/reduction
        if(enemy.elem==casting.elem) mdmg/=2;
        if(enemy.weak==casting.elem) mdmg*=2;
        if(pc.comp.weap.ench==casting.elem) mdmg+=pc.comp.weap.enchb;

        switch(pc.comp.iab)
        {
        case 0://char
            break;
        case 1:
            break;
        case 9://poison
            {
                cout<<pc.comp.name<<" poisons their weapon and attacks!\n";
                bool psch=roll20(pc.comp.dex);
                if(psch) {enemy.status="poisoned"; cout<<"The enemy has been poisoned!\n";}
                else cout<<"The poison fails to enter the wound.\n";
            }
            break;
        }
        enemy.hp-=mdmg;
        cout<<pc.comp.name<<" casts "<<casting.name<<", dealing "<<mdmg<<" damage! Enemy HP: "<<enemy.hp<<"\n";
    }
    cout<<"\n";
    return enemy;
}

void eat()
{
	int num;

	cout<<"Eat item from which inv slot?\n";
	num=check_int();;

	if( pc.inv[num].type != "food" && pc.inv[num].name != "pot" ) { prln("You come dangerously close to breaking a tooth\nbefore deciding what you're trying to eat is inedible."); return; }

        if(pc.inv[num].ench=="whole")
        {
            cout<<"You eat half the " << pc.inv[num].name << ", saving the rest for later.\nIt restores some health.\n";
            pc.hp+=pc.inv[num].enchb;
            if (game_settings.survival) pc.setHunger( pc.inv[num].enchb * 2 );
            pc.inv[num].ench="half";
        }
        else if(pc.inv[num].ench=="half")
        {
            cout<<"You finish the rest of the " << pc.inv[num].name << ". It restores some health.\n";
            pc.hp+=pc.inv[num].enchb;
            if (game_settings.survival) pc.setHunger( pc.inv[num].enchb * 2 );
            pc.inv[num]=eq[0];
        }
        else
		{
            cout<<"You eat the "<<pc.inv[num].name<<". It restores some health.\n";
            pc.hp+=pc.inv[num].enchb;
            pc.mp+=pc.inv[num].atkb;
            if (game_settings.survival) pc.setHunger( pc.inv[num].enchb  * 2 );
            pc.inv[num]=eq[0];
		}
	if(pc.inv[num].name=="pot")
    {
        if(pc.inv[num].ench!="none")
        {
            cout<<"You eat the stew. It restores some health.\n";
            pc.hp+=pc.inv[num].enchb;
            pc.inv[num].ench="none";
            if (game_settings.survival) pc.setHunger( pc.inv[num].enchb );
            pc.inv[num].enchb = 0;
        }
    }
	else if(pc.inv[num].name=="medicinal herb")
	{
		cout<<"You chew the wad of bitter leaves and swallow.\n";
		cout<<"It heals 1 HP.\n";
		pc.hp++;
		pc.inv[num]=eq[0];
		bool hch=roll(5);
		if(hch>3)
		{
			cout<<"You begin to feel much healthier.\nYour status has been restored!\n";
			pc.status="none";
		}
	}
	else if(pc.inv[num].name=="snowflower")
	{
		cout<<"Your tongue tingles with sudden cold.\n";
		cout<<"You feel a little more energetic.\n";
		pc.mp+=pc.inv[num].enchb;
		pc.inv[num]=eq[0];
	}
	else if(pc.inv[num].name=="torchweed")
	{
		cout<<"Your senses are assaulted by a powerful, acrid taste.\n";
		cout<<"You feel a little vitality return to your limbs.\n";
		pc.hp+=pc.inv[num].enchb;
		pc.inv[num]=eq[0];
	}

	pc.showHealth();
	pc.showMana();
}

void drink()
{

	int container, y;
	string contents;

    cout <<"Enter a number to drink from an inventory slot:\n";
	y=check_int();

	contents=pc.inv[y].ench;

				string cont=contents;
				if(cont!="none"&&pc.inv[y].type=="container")
				{
					cout<<"You open the vessel and drink the "<<cont<<".\n";
					pc.inv[y].ench="none";

					if(cont=="water")
					{
						cout<<"You feel refreshed.\n";
						if(pc.mp<pc.mpmax)
							pc.mp++;
                        pc.thirst = 100;
					}
					if(cont=="milk")
					{
						cout<<"You feel refreshed.\n";
						if(pc.hp<pc.mpmax)
							pc.hp+=rand()%10+2;
                        if (game_settings.survival) pc.setThirst(50);
					}
					if(cont=="swampwater")
					{
						cout<<"You feel...horrible.\n You spew the gritty, foul water onto the ground but it's too late.\nYou have been poisoned!\n";
						pc.status="poisoned";

					}
					if(cont=="ale")
					{
						cout<<"\nIt is a hearty blend of bitter and sweet, and warms your belly.\nYou feel a little stronger...and a little dizzy, too.\n";
						pc.hp+=3;
						pctox=10;
                        if (game_settings.survival) pc.setThirst(-10);

						if(pc.race!="Dwarven")
						{
						    if(pc.status=="drunk")
                            {
                                cout<<"Ah- that was one too many-\nThe world reels around for a moment before you hit the ground with a thud.\nEverything goes dark.\n\n";
                                timen=0;
                                day++;

                                for(int x=0;x<4;x++)
                                {
                                    Sleep(1);
                                    cout<<".\n";
                                }
                                cout<<"You awaken with a groan.\n";
                                cout<<"\nYou are hungover.\n";
                                pc.status="hungover";
                                pc.hpmax--;
                            }
                            else if(pc.status=="mildly intoxicated")
                            {
                                cout<<"\nThe world begins to lean crazily around you...You are now drunk.\n";
                                pc.status="drunk";
                            }
                            else if(pc.status=="hungover")
                            {
                                cout<<"Turns out the 'hair of the dog' isn't just a myth...You feel a little better.\n";
                                pc.status="OK";
                                pc.intl++;
                                pc.str--;
                                pc.hpmax--;
                            }
                            else
                            {
                                cout<<"\nYou are now mildly intoxicated.\n";
                                pc.status="mildly intoxicated";
                                pc.str++;
                                pc.intl--;
                            }
						}//if not dwarven
					}
					else if(cont=="mystic sap")
					{
						int r=roll(8);
						cout<<"\nYou feel an invigorating warmth flood\nyour body. Suddenly, the energy of the Forest\nGuardian courses through your veins. For a\nflickering instant, you see all living things\non the earth, connected by faint glowing threads.\nThe trees shine too brightly to look at.\nAfter a moment, the vision passes, but the power awoken\nremains.\n\nYour ";
						switch(r)
						{
							case 1: {pc.atk++; cout<<"attack";} break;
							case 2: {pc.def++; cout<<"defense";} break;
							case 3: {pc.str++; cout<<"strength";} break;
							case 4: {pc.dex++; cout<<"dexterity";} break;
							case 5: {pc.intl++; cout<<"intellect";} break;
							case 6: {pc.lck++; cout<<"luck";} break;
							case 7: {pc.hpmax+=5; cout<<"maximum health";} break;
							case 8: {pc.mpmax+=5; cout<<"maximum mana";} break;
						}
						if (game_settings.survival) pc.setThirst(30);
						cout<<" has been increased!\n";
						pc.status="woke";
					}
					else if (cont=="firefly")
						cout<<"You cough and splutter, realizing your mistake, but it's too late.\nYour stomach feels kind of warm, and you swear it's glowing a little.\n";
					else if(cont=="poison")
					{
						cout<<"You immediately start to feel dizzy.\nThe blood drains from your face, and the ground rushes up at you.\nYou lay helpless as your life ebbs away.\n\n...\n\nWhat did you think would happen?\n";
						pc.hp-=pc.hpmax;
					}
					else if(cont=="strength-potion")
                    {
                        cout<<"New strength floods your body. Your muscles swell a little bit.\n";
                        pc.str++;
                    }
                    else if(cont=="dexterity-potion")
                    {
                        cout<<"A burst of energy fills your veins. You feel your heart beating a little faster.\n";
                        pc.dex++;
                    }
                    else if(cont=="intellect-potion")
                    {
                        cout<<"Your mind suddenly expands, understanding the world more clearly.\n";
                        pc.intl++;
                    }
                    else if(cont=="luck-potion")
                    {
                        cout<<"A warm glow suffuses your body. You feel vibrantly optimistic.\n";
                        pc.lck++;
                    }
				}
				else cout<<"You can't drink that!\n";
}

void attackDesc(string type, string wnm)
{
    string atp[3];
    string atc="null";
    if (type=="bow"&&pc.arrows<=0) type="blunt";

    if(type=="sword"||type=="blade")
    {
        atp[0]="make a horizontal slash"; atp[1]="change your grip and thrust"; atp[2]="raise your blade over head and chop";
    }
    else if(type=="bow")
    {
        if(pc.arrows>0)
            atc="take aim and fire";
    }
    else if(type=="blunt"||type=="staff")
    {
        atp[0]="extend your reach and swing"; atp[1]="make several jabs"; atp[2]="aim low and sweep";
    }
    else if(type=="null"||type=="unarmed"||type=="none")
    {
        wnm="fist";
        atp[0]="wind back and swing hard"; atp[1]="swiftly jab"; atp[2]="step forward and lunge";
    }
    if(atc=="null"&&atc!="fire") atc=atp[rand()%3];
    cout<<"\nYou "<<atc<<" at the enemy with your "<<wnm<<"!\n";
}

void deathScreen()
{
    cout<<"\n";
}

void Death()
{
    if (game_settings.diff_desc == "Sandbox"){
        prln("What's this? Some form of magic returns you to your feet unharmed.");
        pc.hp = pc.hpmax;
        if (game_settings.survival) {pc.hunger = 100; pc.thirst = 100;}
        return;
    }

    prln("Your vision fades to black.");

    cout<<"\nContinue? (y / n)\n";
    cin>>pc.inp;
    if(pc.inp == "n")
    {
        cout<<"\n\nYou release your grip on life and relinquish unto death.\n";
        cout<<"\n";
        pc.inp = "quit";
        pc.status = "dead";
        return;
    }

    Sleep(1300);
    prln("You slowly awaken to the sound of rushing water.\nYou open your eyes to a blinding white, sunless sky.");
    Sleep(3000);
    prln("For a while, you lay still, ice-cold water flowing around you.");
    Sleep(5000);
    prln("You sit up, looking around.\n\nA vast expanse of shallow water flows around islands of bone-white trees.\nChill fog obscures the distance.");
    Sleep(4000);

    string lost_d[5];
    lost_d[0] = {"You continue walking through the empty void, freezing water chilling you to the bone."};
    lost_d[1] = {"A dark shape wriggles by, just under the surface of the water."};
    lost_d[2] = {"A mournful weeping echoes in the distance."};
    lost_d[3] = {"The trees thin out and disappear completely,\nleaving you sloshing through an empty flatland of eddying water."};
    lost_d[4] = {"A dark silhouette approaches from the fog, then begins to fade before you can get a glimpse."};

    int time_of_death = roll(10);
    int seconds[5] = {4000, 4000, 5000, 5000, 6000};
    for(int x = 0; x < time_of_death; x++)
    {
        prln(lost_d[rollfz(5)]);
        Sleep(seconds[rollfz(5)]);
    }

    prln("You stop suddenly, feeling a spot of warmth in the water.");
    Sleep(seconds[0]);
    prln("You step closer, detecting a faint, shimmering light above the swirling current...");
    Sleep(seconds[0]);
    prln("You gently put your hand into the light. It disappears.\nHolding your breath, you follow after, walking into the light.");
    for(int x=0; x<roll(5); x++) {cout<<"...\n"; Sleep(4100);}
    prln("Your senses are momentarily overwhelmed.\nBright light - different, warmer light -\nand a sweet breeze of fresh air caress your face.");
    prln("You hear a voice call out, and look up.\nThe familiar streets of your village welcome you.");
    prln("Arana waves from her porch, smiling, and you wave back, taking in a deep breath.");

    pc.hp = pc.hpmax;
    pc.area = loc[1][10];
    pc.area.loc_flora = plant[0];
    pc.area.loc_fauna = anim[0];

    if(rollfor(4, pc.karma))
    {
        prln("You seem to have lost something...");

        switch(roll(3))
        {
            case 1: pc.xp = 0; break;
            case 2: pc.gp = 0; break;
            case 3: {int it = roll(10); pc.inv[it] = eq[0];}
        }
    }

    pc.hunger = 100;
    pc.thirst = 100;
}

void classup()
{
    //Player unlocks a new class at levels 5 and 10 by choosing between two items

	cout << "\nA tiny wizard in black robes and a \npointed hat appears in a puff of smoke.\n\n";
	cout << "'Hello!' he says, grinning madly.\n'Hello,' you reply warily.\n\n";
	cout << "'Seems to me,' he says, tapping the side of a very\nlong nose with a finger, 'You've made\n considerable headway here. How about an upgrade?'\n\n";
	cout << "He waves his wand and with a popping noise (and\nrather more glitter than one would expect) two objects \nappear before him.\n\n";

	string classit[2];
	string classnxt[2];
	int strup[2];
	int dxup[2];
	int intup[2];
	int lckup[2];
	int num;
	int ccol;
	int ccol2;
	if(pc.clas=="Adventurer")
	{
		classit[0]="arrow";
		ccol=2;
		classnxt[0]="Hunter";
		strup[0]=1;
		dxup[0]=1;
		intup[0]=0;
		lckup[0]=1;
		classit[1]="sword";
		ccol2=4;
		classnxt[1]="Fighter";
		strup[1]=2;
		dxup[1]=1;
		intup[1]=0;
		lckup[1]=0;
	}
	else if(pc.clas=="Scholar")
	{
		classit[0]="book";
		ccol=1;
		classnxt[0]="Mage";
		strup[0]=0;
		dxup[0]=0;
		intup[0]=3;
		lckup[0]=0;
		classit[1]="leaf";
		ccol2=10;
		classnxt[1]="Herbalist";
		strup[1]=0;
		dxup[1]=0;
		intup[1]=1;
		lckup[1]=2;
	}
	else if(pc.clas=="Wanderer")
	{
		classit[0]="harp";
		ccol=6;
		classnxt[0]="Bard";
		strup[0]=0;
		dxup[0]=0;
		intup[0]=2;
		lckup[0]=1;
		classit[1]="mask";
		ccol2=8;
		classnxt[1]="Thief";
		strup[1]=1;
		dxup[1]=1;
		intup[1]=0;
		lckup[1]=1;
	}
	else if(pc.clas=="Bard")
	{
		classit[0]="rose";
		ccol=12;
		classnxt[0]="Courtesan";
		strup[0]=0;
		dxup[0]=0;
		intup[0]=2;
		lckup[0]=2;
		classit[1]="wasp";
		ccol2=10;
		classnxt[1]="Duelist";
		strup[1]=1;
		dxup[1]=2;
		intup[1]=0;
		lckup[1]=1;
	}
	else if(pc.clas=="Thief")
	{
		classit[0]="dice";
		ccol=13;
		classnxt[0]="Trickster";
		strup[0]=1;
		dxup[0]=1;
		intup[0]=1;
		lckup[0]=1;
		classit[1]="knife";
		ccol2=4;
		classnxt[1]="Bandit";
		strup[1]=2;
		dxup[1]=2;
		intup[1]=0;
		lckup[1]=0;
	}
	else if(pc.clas=="Hunter")
	{
		classit[0]="wolf";
		ccol=COL_GREY;
		classnxt[0]="Ranger";
		strup[0]=2;
		dxup[0]=2;
		intup[0]=0;
		lckup[0]=0;
		classit[1]="skull";
		ccol2=COL_RED;
		classnxt[1]="Barbarian";
		strup[1]=4;
		dxup[1]=0;
		intup[1]=0;
		lckup[1]=0;
	}
	else if(pc.clas=="Fighter")
	{
		classit[0]="flame";
		ccol=COL_ORANGE;
		classnxt[0]="Spellsword";
		strup[0]=2;
		dxup[0]=0;
		intup[0]=2;
		lckup[0]=0;
		classit[1]="shield";
		ccol2=9;
		classnxt[1]="Knight";
		strup[1]=2;
		dxup[1]=0;
		intup[1]=1;
		lckup[1]=1;
	}
	else if(pc.clas=="Mage")
	{
		classit[0]="crystal";
		ccol=COL_CYAN;
		classnxt[0]="Wizard";
		strup[0]=0;
		dxup[0]=0;
		intup[0]=4;
		lckup[0]=0;
		classit[1]="moon";
		ccol2=COL_WHITE;
		classnxt[1]="Necromancer";
		strup[1]=1;
		dxup[1]=0;
		intup[1]=3;
		lckup[1]=0;
	}
	else if(pc.clas=="Herbalist")
	{
		classit[0]="star";
		ccol=14;
		classnxt[0]="Priest";
		strup[0]=0;
		dxup[0]=0;
		intup[0]=1;
		lckup[0]=3;
		classit[1]="tree";
		ccol2=COL_GREEN;
		classnxt[1]="Druid";
		strup[1]=0;
		dxup[1]=1;
		intup[1]=3;
		lckup[1]=0;
	}

	bool selection_made = false;
	pc.inp = "null";
	while (!selection_made)
    {
        cout << "You take the...\n\n";
        cout<<"... "; colSet(ccol, classit[0]); cout <<"\n";
        cout<<"... "; colSet(ccol2, classit[1]); cout<<"\n";

        cout << "\n";
        cout << ">";
        cin >> pc.inp;

        if(pc.inp == classit[0]) {selection_made = true; num = 0;}
        else if(pc.inp == classit[1]) {selection_made = true; num = 1;}
        else if(pc.inp == "1") {selection_made = true; num = 0;}
        else if(pc.inp == "2") {selection_made = true; num = 1;}
        else cout << "\nYou hesitate for a moment before making your choice.\n\n";
    }

	pc.clas=classnxt[num];
	pc.tat=classit[num];
	pc.str+=strup[num];
	pc.dex+=dxup[num];
	pc.intl+=intup[num];
	pc.lck+=lckup[num];

	cout<<"\n'A "<<classnxt[num]<<", hmm?' he says, his eyes glittering.\n'Interesting.'\n\n";
	cout<< "Your hand passes straight through the " << classit[num] << ".\nIt disappears in a burst of colorful smoke,\nreappearing as a tattoo on the back of your hand.\n";

	cout << "\nThe tiny wizard claps his hands and vanishes in a cloud of ";
	string cloud[7] = {"bees", "smoke", "confetti", "vapor", "dust", "glitter", "sulphurous gas"};
	cout << cloud[rand()%7] << ".\n";
	cout << "You hear a chortling voice echoing in the distance.\n";
	cout << "\n'I'll see you again sooooon...'\n";
}

void lvlup()
{
	int stp;

	if(pc.xp>=pc.xpnxt)
	{
		stp=pc.lvl/10;
		if (stp<=0) stp=1;

		pc.lvl++;
		pc.xp-=pc.xpnxt;
		pc.xpnxt*=1.2;

		pc.hpmax+=pc.str+1;
		pc.hp=pc.hpmax;
		pc.mpmax+=pc.intl+1;
		pc.mp=pc.mpmax;

		cout<<"\nYou feel you have attained a new height of mastery...\n\n";
        if(game_settings.sound) mciSendString(sdLvlup.c_str(), NULL, 0, NULL);
		do
		{
            cout <<"Level Up!\n\n";
            cout <<"Choose a stat to raise (points remaining: " << stp << "):\n";
            colSet( 4, "\nStrength: " ); cout << pc.str;
            colSet( 2, "\nDexterity: "); cout << pc.dex;
            colSet( 1, "\nIntellect: "); cout << pc.intl;
            colSet( 14, "\nLuck: "); cout << pc.lck << "\n\n";
            pc.inp = pc.getInps();

        cout<<"\n";
		if(pc.inp=="str" || strCase(pc.inp, "upper") == "Strength" || pc.inp == "1")
		{
			cout<<"Strength: "<<pc.str<<" -> ";
			pc.str++;
			cout<<pc.str<<"\n";
			stp--;
		}
		else if(pc.inp=="dex" || strCase(pc.inp, "upper") == "Dexterity" || pc.inp =="2")
		{
			cout<<"Dexterity: "<<pc.dex<<" -> ";
			pc.dex++;
			cout<<pc.dex<<"\n";
			stp--;
		}
		else if(pc.inp=="int" || strCase(pc.inp, "upper") == "Intellect" || pc.inp == "3")
		{
			cout<<"Intellect: "<<pc.intl<<" -> ";
			pc.intl++;
			cout<<pc.intl<<"\n";
			stp--;
		}
		else if(pc.inp=="lck" || strCase(pc.inp, "upper") == "Luck" || pc.inp == "4")
		{
			cout<<"Luck: "<<pc.lck<<" -> ";
			pc.lck++;
			cout<<pc.lck<<"\n";
			stp--;
		}
		else cout<<"That's not a stat you can raise!\n";
		} while(stp!=0);
		pc.carrywtmax=20+pc.str;

		if(pc.lvl%5==0&&pc.lvl<11)
			classup();

		int unlpts=1;
		if(unlpts<=0)
			unlpts=1;
		int ct=0;
		int spells_unlocked=0;

		for(int x=0;x<70;x++)
		{
			if( pc.clas == splist[x].clas && pc.lvl >= splist[x].req && !splist[x].avail ) splist[x].avail = true;
			if ( splist[x].avail ) spells_unlocked++;
		}

		do
		{
			cout<<"\nSpells/Abilities available to unlock:\n\n";

		for(int x=0;x<70;x++)
			if( splist[x].avail == true && splist[x].unlock == false )
			{
				cout << strCase(splist[x].name, "upper") <<" : " << splist[x].desc << "\n";
				ct++;
			}
			if(unlpts>0&&ct>0)
			{
				cout<<"\nWhich spell do you want to unlock?\n\n";

				int num;
				pc.inp = pc.getInps();

				for(int i = 0; i < 70; i++){
                    if( pc.inp.length() < 3 )
                        if ( stoi(pc.inp) == splist[i].id ) {num = i; break;}
                    if (pc.inp == splist[i].name) {num = i; break;}
				}

				splist[num].unlock=true;
				unlpts--;
				ct--;
				cout<<"You learned '"<<splist[num].name<<"'!\n";

				//Spell tutorial
				if(spells_unlocked==1)
                {
                    cout<<"\nLooks like you've unlocked a new spell or ability!\n\n";
                    cout<<"Whenever you want to use it, type 'cast' and then\nthe name of the ability.\n";
                }
			}
			else if(ct==0)
			{
				cout<<"No new abilities!\n";
				unlpts=0;
			}
		}while(unlpts>0&&ct>0);
		cout<<"\n";
	}
}

enem createEnemy()
{
    //Chooses an enemy from the area list
    int eroll = rand()% 100 + 1;
    int enemy_id = 0;
	switch(eroll){case 1 ... 50: enemy_id = 0; break; case 51 ... 80: enemy_id = 1; case 81 ... 100: enemy_id = 2;}
    enem e = elist[ pc.area.enemies[ enemy_id ] ]; //Set temporary enemy

    //Sets enemy level and alters stats to match
    e.setLevel(pc.lvl);

    //Give certain enemies titles based on their stats
    string title = "none";

    //Warriors / soldiers
    string w_def_titles[5] ={"Defender", "Guard", "Sentinel", "Warden", "Shielder"};
    string w_atk_titles[5] = {"Berserker", "Assassin", "Slayer", "Elite", "Vanguard"};
    string w_dex_titles[5] = {"Rogue", "Scout", "Infantry", "Hunter", "Tracker"};

    string def_titles[5], atk_titles[5], dex_titles[5];
    if(e.type == "warrior" && e.name != "Spectral Warrior" && e.name != "Bandit") {
        if(e.def > elist[e.id].def + (e.lvl/2) ) {
            if (e.name == "Paladin") e.ab = splist[48]; //Paladin switches between holyshield and blessedblade
            e.name = e.name + " " + w_def_titles[rollfz(5)];
        }
        else if(e.atk > elist[e.id].atk + (e.lvl/2) ) {
            if (e.name == "Paladin") e.ab = splist[47];
            e.name = e.name + " " + w_atk_titles[rollfz(5)];
        }
        else if(e.dex > elist[e.id].dex + (e.lvl/5) ) e.name = e.name + " " + w_dex_titles[rollfz(5)];
    }

    if(e.name == "Slime"){
        if( e.statIsHighest() == "str" ) { e.name = "Red " + e.name; e.elem = "fire"; }
        else if ( e.statIsHighest() == "dex" ) { e.name = "Green " + e.name; e.elem = "poison"; }
        else if ( e.statIsHighest() == "int" ) { e.name = "Blue " + e.name;  e.elem = "water"; }
        else if ( e.statIsHighest() == "lck" ) { e.name = "Yellow " + e.name;  e.elem = "light"; }
    }

    e.alive = true;

    return e;
}


void combat()
{
    //Create a temporary enemy
	enem enemy;
	bool compturn;

	//Load local enemy or create a new one
	if (pc.area.foe.alive) {enemy = pc.area.foe;}
	else
	{
        enemy = createEnemy();
        pc.area.foe = enemy;
        pc.area.foe.alive = true;
        enemy.status = "ok";
    }

    //End "the weak" encounter if enemy is an NPC-type
    if ( m_data.weak_nearby && (enemy.type == "NPC" || enemy.type == "NPC_warrior") ){ prln("...Nevermind. it was just a brawl between some locals."); return; }

    //Change console caption
    string battle_caption = "Combat - Lv. " + to_string(enemy.lvl) + " " + enemy.name;
    setConCap( battle_caption );

    //triggered by 'Silencing the Source'
    if(enemy.name=="Crazed Arcanist"&&loc[4][7].npcs[1].q.lvl>=1)
    {
        enemy.lvl+=rand()%10+5;
        cout<<"\n   ->It seems you've found the rogue mage Azelfoff mentioned...Time to get to work.\n\n";
    }

	//set damage ranges, initialize temporary damage variables
	int pdmax=(pc.atk-enemy.def)+pc.str;
	if(pdmax<=0)pdmax=1;
	int pdmg;

	int edmax=(enemy.atk-pc.def)+enemy.str;
	int edcmax=(enemy.atk-pc.comp.def)+enemy.str;
	if(edcmax<=0)edmax=1;
	if(edmax<=0)edmax=1;

	//Derived stats
	int edmg;
	int pcrit;
	int ecrit;
	int pdodge;
	int edodge;
	int estag;
	bool balance;
	bool use;

	//create temps for base stats, to return at end in case of boosting
	equipment weap=pc.weap;
	if(pc.weap.name =="empty") pc.weap.name = "fist"; //If player is unarmed, temporarily set default weapon name
	equipment arm=pc.arm;
	equipment acc=pc.acc;
	int batk=pc.atk;
	int bdef=pc.def;
	int bstr=pc.str;
	int bdex=pc.dex;
	int bint=pc.intl;
	int blck=pc.lck;

	//Buffs/debuffs
	int amod, dmod, strmod, dexmod, intmod, lckmod;

	//Attack descriptions
	string ad[3];

	//If the enemy is a boss, play an intro
	string intro, outro;

	if(enemy.type == "boss")
    {
        if(enemy.name == "Goblin Chieftain")
        {
            //Intro
            cout<< " ";
            //Set attack descriptions
            ad[0] = " "; ad[1] = " "; ad[2] = " ";
        }
        if(enemy.name == "Forest Guardian")
        {
            intro = "\nA cacophony of rustling, creaking branches overhead catches your attention.\n\nThe gargantuan tree at the center of the clearing begins to stir,\ntearing its roots from the earth in a shower of dirt.\nIts distant leafy crown swaying far above the forest canopy,\nit emits a bone-jarring rumble and begins to lumber toward you...\n\n";
            outro = "\nThe towering tree groans and cracks, reeling drunkenly;\nyou fall backwards with a yell of surprise, thrown\nby the thunderous crash of the great forest giant's collapse.";
            cout << intro;
            ad[0] = "clobbers you with a massive branch"; ad[1] = " "; ad[2] = " ";
        }
        if(enemy.name == "Ice Golem")
        {
            cout<< " ";
            ad[0] = " "; ad[1] = " "; ad[2] = " ";
        }
        if(enemy.name == "Dark General")
        {
            cout<< " ";
            ad[0] = " "; ad[1] = " "; ad[2] = " ";
        }
        if(enemy.name == "Underworld Empress")
        {
            cout<< " ";
            ad[0] = " "; ad[1] = " "; ad[2] = " ";
        }
        if(enemy.name == "Ascendant Queen")
        {
            cout<< " ";
            ad[0] = " "; ad[1] = " "; ad[2] = " ";
        }
        if(enemy.name == "Ancient Dragon")
        {
            cout<< " ";
            ad[0] = " "; ad[1] = " "; ad[2] = " ";
        }
    }

    //Set attack descriptions for other types
    if(enemy.type == "amorphous") {ad[0] = "swipes at you with an appendage"; ad[1] = "tackles you bodily"; ad[2] = "charges at you";}
    if(enemy.type == "beast") {ad[0] = "slashes with razor-sharp claws"; ad[1] = "bites you viciously"; ad[2] = "launches into you with a flying leap";}
    if(enemy.type == "warrior" || enemy.type == "NPC_warrior") {ad[0] = "slashes with their weapon"; ad[1] = "catches you off guard"; ad[2] = "weaves through your guard and stabs you swiftly";}
    if(enemy.type == "fire") {ad[0] = "scatters embers in your direction"; ad[1] = "ignites the ground underneath you"; ad[2] = "spits a glob of molten rock at you";}
    if(enemy.type == "water") {ad[0] = "lashes at you with liquid tendrils"; ad[1] = "batters you with rippling waves"; ad[2] = "slams into you with a torrent of water";}
    if(enemy.type == "plant") {ad[0] = "launches several woody projectiles"; ad[1] = "lashes out with a leafy tendril"; ad[2] = "wraps you in writhing vines";}
    if(enemy.type == "electric") {ad[0] = "connects to you with a prolonged\nstream of electricity"; ad[1] = "snaps out with several flickering bolts"; ad[2] = "shocks you with a powerful field of static";}
    if(enemy.type == "archer") {ad[0] = "fires a swift arrow"; ad[1] = "takes quick aim and lets fly"; ad[2] = "closes in, dodging your counter and\nstabbing you with an arrow";}

	if(game_settings.sound) mciSendString(sdAtk.c_str(), NULL, 0, NULL);

	cout<<"\nLook out! "<<aoran(enemy.name, true)<<enemy.name<<" attacks!\n";
	cout<<" (1) Fight\n (2) Flee\n";
	pc.inp = pc.getInps();
	if(pc.inp=="1")
	{
		cout<<"You engage the enemy.\n";
	}
	else
	{
        if(m_data.weak_nearby) {cout<<"You can't run away when you're protecting something important!\n";}
		else if(pc.dex>=enemy.dex)
		{
			cout<<"You deftly evade the enemy until\nit leaves off trying to kill you.\n\n";
			return;
		}
		else
			cout<<"Couldn't flee!\n\n";
	}

		cout << "\n" << battle_caption << "\n";
		cout << "\nHP: " << enemy.hp << "  Atk: " << enemy.atk << "  Def: " << enemy.def << "\n\n";
		cout<<"  Quick battle commands:\n(hp/mp/ap) health, mana, antidote potion\n(a) attack\n(c) cast/use ability\n(f) flee\n";
		if (pc.acc.subt == "shield") cout << "(b) block/defend\n";
		cout << "\n";
        if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\sword1.mp3", NULL, 0, NULL);

        //+===================+
        //
        // - Main combat loop -
        //
        //+===================+
		do
		{
		    //Set player turn (not companion)
		    compturn=0;

			//status effects
			bool ppara=false;
			if(pc.status=="paralyzed")
			{
				int pparach=rand()%100+1;
				if(pparach<51)
					ppara=true;
			}
			if(pc.status == "poisoned")
			{
				int psndm = roll(5);
				if (game_settings.diff_lvl > 5) psndm += game_settings.diff_lvl;
				pc.hp -= psndm;
				cout<<"You take "<<psndm<<" damage from poison!\n";
			}

			//Clear player input
			pc.inp="";

			//+-----------------+
			//     Get Input
			//+-----------------+

			//Check first for paralysis
			//or multi-turn actions
			if(ppara==false&&pc.casting.elem!="shield")
			{
			    //Get input
			    cout << "What will you do?\n";
				pc.inp = pc.getInps();
			}
			else if(ppara==true)
				cout<<"You are paralyzed! You can't move!\n";
			else if(pc.casting.elem=="shield")
			{
				pc.casting.elem="none";
				pc.def/2;
			}

			//Parse input
			if(pc.inp=="attack"||pc.inp=="a")
			{
				pc.weap.showAction("attack");
				edodge=rand()%100+1;

				if(edodge>enemy.dex)
				{
				    string s_dmg_range[3] = {"barely any", "moderate", "considerable"};
                    string s_dmg;

                    //Roll for critical hit
					pcrit=rand()%100+1;
					//Critical damage
					bool crit = 0;
					if(pc.lck>=pcrit)
					{
					   crit = 1;
                        pdmg=rand()%(pdmax*3)+1;
						string crit[3]={
						    "A spray of blood spatters the dirt\nand your foe reels miserably.\n",
						    "Your enemy staggers back, clutching an open wound.\n",
						    "Your opponent hits the ground with a loud thud\nand rolls painfully to their feet.\n"};
                        cout << crit[rand()%3];

                        cout<<"Your attack deals ";
						if( game_settings.combat_display != "text" ) cout << pdmg;
						else cout << "critical";
                        cout << " damage!";
						if(game_settings.sound) mciSendString(sdCrit.c_str(), NULL, 0, NULL);
					}
					//Basic damage
					else {pdmg=rand()%pdmax+1;}
					//Apply damage to enemy
					enemy.hp-=pdmg;
					//Get text output for damage
                    if(!crit){
						if(pdmg <= 2) s_dmg = s_dmg_range[0];
                        else if(pdmg >= pdmax - 1) s_dmg = s_dmg_range[2];
                        else if(pdmg >= pdmax / 2) s_dmg = s_dmg_range[1];

                        cout <<"\nYour attack deals ";
						if(game_settings.combat_display == "text") cout << s_dmg;
						else cout << pdmg;
						cout << " damage;";
                    }
					if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\swordclank3.wav", NULL, 0, NULL);
					//enchantment bonus
					if(pc.weap.ench!="none")
					{
						int enchd=pc.weap.enchb;
						if(pc.weap.ench==enemy.weak)
							enchd*=2;
						if(pc.weap.ench==enemy.elem)
							enchd/=2;
						enemy.hp-=enchd;
						cout<<"\nYour enchanted weapon deals an extra ";
						if(game_settings.combat_display == "text") cout << "burst of";
						else cout << enchd;
						cout << " " << pc.weap.ench << " damage!";
					}

					//Display enemy HP
					if(enemy.hp > 0) enemy.showHealth();
				}//enemy fails dodge
				else {
                        string ed_desc[3]={" ", " ", " "};
                        cout<<"\nThe enemy dodges your wild swing.\n";
                        if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\swordwhip.wav", NULL, 0, NULL);
				}
			}//end of if "attack" command

			else if(pc.inp=="cast"||pc.inp=="c")
            {
					cout<<"Cast which spell?\n";
					cin>>pc.inp;
					for(int x=0; x<68;x++)
						if(splist[x].name==pc.inp)
							pc.casting=splist[x];
					enemy=cast(enemy);
            }
            else if(pc.inp == "b" || pc.inp == "block")
            {
                if(pc.acc.subt == "shield")
                {
                    prln("You ready a defensive stance, bracing for your opponent's next attack.\n");
                }
            }
			else if(pc.inp=="onepunch"||pc.inp=="kamehameha")
			{
			    if(admin==true){
				enemy.alive=false;
				pc.area.foe.alive=false;}
				else cout<<"You try to copy a move you saw on TV, to no avail.\n";
			}//cheat to win
			else if(pc.inp=="hpot"||pc.inp=="hp")
			{
				hpotion();
			}
			else if(pc.inp=="mpot"||pc.inp=="mp")
			{
				mpotion();
			}
			else if(pc.inp=="apot"||pc.inp=="ap")
			{
				apotion();
			}
			else if(pc.inp=="read")
			{
				cout<<"Read item from which inventory slot?\n";
				int num;
				num=check_int();;
				if(pc.inv[num].name=="scroll")
				{
					for(int x=0;x<68;x++)
					{
						if(splist[x].name==pc.inv[num].ench)
						pc.casting=splist[x];
					}
							if(pc.inv[num].price>0)
							{
								cout<<"The parchment shivers and cracks from the release of\nmagical energy.\n";
								pc.inv[num].price--;
							}
							else
							{
								cout<<"The scroll bursts into flame.\nIn seconds it is only ash in the wind.\n";
								pc.inv[num]=eq[0];
							}
							pc.casting.cost=0;
							pc.casting.unlock=true;
							enemy=cast(enemy);
				}
				else
					cout<<"You can't read that now!\n";
			}
			else if(pc.inp=="drop")
			{
				pc.area.loot=pc.weap;
				pc.carrywt-=pc.weap.wt;
				pc.weap=eq[0];
				cout<<"You drop your weapon on the ground.\n";
			}
			else if(pc.inp=="eat")
				eat();
			else if(pc.inp=="drink")
				drink();
			else if(pc.inp=="draw")
			{
				if(pc.sheath==true&&pc.sheathed.name!="empty"&&pc.weap.name=="empty")
				{
					pc.weap=pc.sheathed;
					pc.sheathed=eq[0];
					cout<<"You draw your "<<pc.weap.mat<<" "<<pc.weap.name<<" from its sheath.\n";
				}
				else
					cout<<"You can't do that now!\n";
			}
			else if(pc.inp=="flee"||pc.inp=="f")
			{
			    int dxchk = ( pc.dex - enemy.dex ) + pc.lck;
			    cout<<"dxchk: "<<dxchk<<"\n";

			    //If HP is low, significantly increase flee chance
                if( ( pc.hp < ( pc.hpmax / 2 ) ) && ( pdmax < 3 ) ) dxchk += 10;

			    //Can't flee if engaged in the Weak random encounter
			    if(m_data.weak_nearby) {cout<<"You can't run away when you're protecting something important!\n"; dxchk=0;}

				if( rollfor(dxchk, 10) ) {
                    cout<<"You dodge your enemy's attack and hide\nuntil it loses interest.\n";
                    return; }
				else
					if(!m_data.weak_nearby) cout<<"You try to run but your enemy is faster!\n";
			}
			else if(pc.inp=="sheath")
            {
                if (pc.sheathed.name == "empty"){
                pc.sheathed=pc.weap;
                pc.weap=eq[0];}
                else cout <<"\nThere is already a weapon in your sheath!\n";
            }

            if(pc.comp.name!="empty"&&pc.comp.name!="none"&&pc.comp.dex>=enemy.dex)
            {
                enemy=compFight(enemy);
                compturn=1;
            }


			//Pause for effect
			for (int s = 0; s < game_settings.combat_wait_time; s++)
            {
                cout << ".";
                Sleep(700);
            }
            cout << "\n";


            //====================-
			//-----Enemy Turn---
			//=============-

			//Check enemy vitals
			if(enemy.hp <= 0)
			{
				enemy.alive = false;
				pc.area.foe.alive = false;
			}
			ecrit=rand()%100+1;
			pdodge=rand()%100+1;

			bool epara=false;

			if(enemy.status=="paralyzed") {
                    if( rollfor(1, 2) ) {
                    epara = true;
                    prln("The enemy succumbs to its paralysis...It can't move!");
                }
			}
			if(enemy.status=="poisoned")
			{
				int psnd=rand()%5+1;
				cout<<"\nThe enemy takes "<<psnd<<" damage from poison!\n";
				enemy.hp-=psnd;
			}
			if(enemy.status=="stagger")
			{
				estag++;
				if(estag>1)
				{
					enemy.status="none";
					estag=0;
				}
				else
					cout<<"The enemy is still recovering from being knocked back.\n";
			}

        if( !epara && enemy.status != "stagger" && enemy.alive==true)
        {

            bool attack = enemy.chooseAttack(pc.arm.ench);

        if( !attack ) enemy = ecast(enemy);
        else
        {
            //enemy chooses a target between player and companion
            int tn = rand() % 2;
            //If you don't have a companion the default target is player (0)
            if( pc.comp.name == "empty" || pc.comp.hp <= 0 ) tn = 0;

            //If player was chosen as attack target
            if(tn == 0){

                string attacks[3]{"swipes at you with an appendage", "tackles you bodily", "charges at you"};
			    for(int x=0; x<3; x++) if(ad[x] != " " && ad[x] != "") attacks[x] = ad[x];
                prln("The " + enemy.name + " " + attacks[rand() % 3] + "!\n");

			if(pdodge>pc.dex)
			{
			    string s_edmg_range[3] = {"barely any", "moderate", "considerable"};
			    string s_edmg;

				edmg=rand()%edmax+1;
				if(edmg<=0) edmg=1;

				if(enemy.lck>=ecrit)
				{
				    string crit_desc[3] = {
                        "Your head swims from the stunning blow!",
                        "Pain shoots through you as you feel something crack-",
                        "You fly backwards and land painfully on your spine."
				    };
				    prln(crit_desc[rand()%3]);
					edmg=(edmg*2)+pc.def;
				}

				if( edmg <= 2 ) s_edmg = s_edmg_range[0];
				else if( edmg >= edmax-2 ) s_edmg = s_edmg_range[2];
				else if( edmg >= edmax/2 ) s_edmg = s_edmg_range[1];

				if( enemy.elem == pc.arm.ench )
				{
					int mdef = roll( pc.arm.enchb );

					edmg -= mdef;

					if( edmg <= 0 ) edmg = 1;

					cout << "The " << pc.arm.ench << " aura surrounding your armor blocks ";

					if( game_settings.combat_display != "text" ) cout << mdef;
                        else cout << "some";

					cout << " of the enemy's attack.\n";
				}

				//Assign damage
				pc.hp -= edmg;

				cout<<"The "<<enemy.name<<" hits you for ";
				if( game_settings.combat_display != "text" ) cout << edmg;
				else cout << s_edmg;
				cout << ";\n";

				pc.showHealth(); cout<<"\n";

				if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\swordclank2.wav", NULL, 0, NULL);

				if(enemy.elem == "poison" && pc.status != "poisoned")
				{
					int epch=rand()%10+1;
					if(epch==1)
					{
						pc.status="poisoned";
						cout<<"You've been poisoned!\n";
					}
				}

				//A held faerie prevents a fatal blow
				if (pc.hp <= 0) {
                    for (int i = 1; i < 10; i++){
                        if (pc.inv[i].type == "container" && pc.inv[i].ench == "Faerie")
                        {
                            prln("The enemy wounds you grievously.\nYou stagger weakly, falling to your knees...");
                            Sleep(1400);
                            prln("Somnolic tinkling fills your ears as the world fades to black.");
                            Sleep(700);
                            prln("The fairy in your pack lights the darkness, appearing before you with an expression full of care;\nYou feel a nurturing embrace and the fairy is gone.");
                            prln("You return to your feet, ready to fight again.");

                            pc.hp = pc.hpmax/2;
                            pc.inv[i].ench == "none";
                        }
                    }
				}
			}//player fails dodge chance'
			else cout<<"You dodge the attack!\n\n";
            }//Player was targeted
            else if(tn==1)
            {
            int cdodge=rand()%100+1;
            if( !rollfor(pc.comp.dex, 100) );
			{
			    string attacks[3]{"swipes at "+pc.comp.name+" with an appendage", "tackles "+pc.comp.name+" bodily", "charges at "+pc.comp.name};
			    cout<<"The "<<enemy.name<<" "<<attacks[rand()%3]<<"!\n";
				edmg=rand()%edmax+1;
				if(edmg<=0) edmg=1;
				if(enemy.lck>=ecrit)
				{
					edmg=(edmg*2)+pc.def;
					cout<<"Critical hit!\n";
				}
				if(enemy.elem==pc.comp.arm.ench)
				{
					int mdef=rand()%pc.comp.arm.enchb;
					edmg-=mdef;
					if(edmg<=0)edmg=1;
					cout<<"The "<<pc.comp.arm.ench<<" aura surrounding"<<pc.comp.name<<"'s armor\nblocks "<<mdef<<" of the enemy's attack.\n";
				}
				pc.comp.hp-=edmg;
				cout<<"The "<<enemy.name<<" hits "<<pc.comp.name<<" for "<<edmg<<"; \n"<<pc.comp.name<<" HP: "<<pc.comp.hp<<"\n\n";
				if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\swordclank2.wav", NULL, 0, NULL);
				if(enemy.elem=="poison"&&pc.comp.status!="poisoned")
				{
					int epch=rand()%10+1;
					if(epch==1)
					{
						pc.comp.status="poisoned";
						cout<<pc.comp.name<<" has been poisoned!\n";
					}
				}
				if(pc.comp.hp<=0)
                {
                    if (pc.comp.race == "undead")
                    {
                        prln("That last blow was too much;\nyour undead companion bursts into pieces.");
                        pc.comp = empty1;
                    }
                    else
                    {
                        cout<<"\nYour companion has taken critical damage!\n"<<pc.comp.name<<" slumps to the ground, out of the fight.\n\n";
                        pc.comp.hp=1;
                    }
                }
			}//Companion fails dodge chance
            }//Companion was chosen as target
			}//enem. doesn't use spell
			}//paralysis check
            if(pc.comp.name!="empty"&&pc.comp.name!="none"&&compturn==0)
            {
                enemy=compFight(enemy);
            }
		}while(enemy.alive==true&&pc.hp>0);
		//Death
		if(pc.hp<=0)
		{
		    //Show death description
		    cout<<"\n\nYour enemy strikes you with one last fatal blow,\nand you topple to the ground.\n";

		    //return base stats
		    pc.atk=batk;
			pc.def=bdef;
			pc.str=bstr;
			pc.dex=bdex;
			pc.intl=bint;
			pc.lck=blck;
			pc.weap=weap;
			pc.arm=arm;
			pc.acc=acc;

		    Death();
			return;
		}
		else
		{
		    int sxpg=0;
            int sgpg=0;

            //If during "the Weak" encounter
			if(m_data.weak_nearby)
            {
                m_data.weak_nearby=false;
                cout<<"\nThe beleaguered "<<weak.n<<" rushes up to you in the aftermath of the battle.\n'Thank you! We'd have been lost without you! Here, please, take this gold as thanks!'\n";
                cout<<"Mini-quest complete!\n";
                sxpg=rand()%pc.intl+5;
                sgpg=rand()%pc.lck+pc.karma + 5;
                setKarma(pc.karma+(enemy.lvl/6), false);

                //roll for trophy
                if(!trophy_list[trophyGetID("shield")].unlock&&rollfor(1,100)) trophyGet("shield", 0);
            }

            if(enemy.type == "boss" && outro != "") cout << outro;

            if(game_settings.sound) mciSendString(sdBell.c_str(), NULL, 0, NULL);
			cout<<"\nVictory!\n";


            //calculate winnings
			int xpg=rand()%(enemy.xpdr+pc.intl)+1;
			int gpg;

			do {gpg=rand()%(enemy.gpdr+pc.lck)+1;}while(gpg>100);
			xpg+=sxpg;
			gpg+=sgpg;

			cout<<"Gained "<<xpg<<" XP, "<<gpg<<" gold!\n\n";
			pc.xp+=xpg;
			pc.gp+=gpg;
			//Companion XP
			if(pc.comp.name!="empty") pc.comp.xp+=rand()%(pc.comp.intl+enemy.xpdr)+1;

			elist[enemy.id].kills++;

			//item drop roll
			int itid;
			//Drop chance% = base 5% + 2 x luck + 10% of karma + enemy level
			int itdch = 5 + ( (2*pc.lck) + (pc.karma/10) ) + enemy.lvl;
			bool drop = rollfor(itdch, 100);
			//Items are guaranteed for boss fights
			if(enemy.type == "boss") drop = true;
			if(drop)
            {
                switch( roll(100) )
                {
                    case 1 ... 55: itid=enemy.itdr[0]; break; //Common drop: 55%
                    case 56 ... 85: itid=enemy.itdr[1]; break; //Uncommon: 30%
                    case 86 ... 100: itid=enemy.itdr[2]; break; //Rare: 15%
                    default: itid = enemy.itdr[0]; break;
                }

                equipment temp = eq[itid];
                //Make sure drop isn't empty
                if (temp.name != "empty") {
                    cout << "The enemy dropped " << aoran( temp.showName(), false);
                    cout << temp.showName() << "!\n";

                    pickup(temp);
                }
            }
            cout << "\n";

            if(!trophy_list[trophyGetID(enemy.name)].unlock&&rollfor(1,100)) trophyGet(enemy.name, 0);

			//return base stats
			pc.atk=batk;
			pc.def=bdef;
			pc.str=bstr;
			pc.dex=bdex;
			pc.intl=bint;
			pc.lck=blck;
			pc.weap=weap;
			pc.arm=arm;
			pc.acc=acc;

			//Karma checks
			if (enemy.type == "NPC" || enemy.type == "NPC_warrior") {
                    setKarma(pc.karma-10, 0);
                    prln("It seems you've slain an innocent creature.\nYou feel a little guilty.");
			}
			if (enemy.name == "Dark Knight" || enemy.name == "Dark Paladin" || enemy.name == "Bandit" || enemy.name == "Vampsect" || enemy.name == "Broodmother") {
                    setKarma(pc.karma, true);
                    prln("It seems you've slain a wicked-hearted being.\nYou feel a sense of liberation.");
			}

			//quest triggers

			//Lending a farm hand
			if(enemy.name=="Rabid Wolf"&&loc[2][6].npcs[0].q.lvl==4)
            {
                loc[2][6].npcs[0].q.lvl=5;
                cout<<"\n   ->You've scared off the wolf pack.\n";
            }
            //piercing the darkness
			if((elist[7].kills+elist[12].kills)>=10&&loc[4][5].npcs[1].q.lvl>=1&&loc[4][5].npcs[1].q.comp==false)
            {
                loc[4][5].npcs[1].questcond=true;
                cout<<"\n   ->The Dark Knights have fallen back. Commander Gareth should be pleased.\n";
            }
            //Ritual Most Profane
			if(enemy.name=="Rampaging Demon") loc[4][7].npcs[2].questcond=true;
			//Silencing the Source
			if(enemy.name=="Crazed Arcanist"&&loc[4][7].npcs[1].q.lvl>=1)
            {
                loc[4][7].npcs[1].questcond=true;
                cout<<"\n   ->You've carried out what was obviously a hit. You should return to Azelfoff.\n";
            }
            //Strictly Business
            if(enemy.name=="Bandit"&&loc[6][5].npcs[1].q.lvl>=1&&enemy.kills>=10&&loc[6][5].npcs[1].q.comp==false)
            {
                loc[6][5].npcs[1].questcond=true;
                cout<<"  ->You've taken care of the Assassin's Guild's bandit problem.\nReturn to Hakon for your payment.\n";
            }
            //Primal: Part One
			if(enemy.name == "frillneck" && elist[enemy.id].kills == 5 && loc[8][1].npcs[0].q.lvl == 2 && !loc[8][1].npcs[0].questcond)
            {
                loc[8][1].npcs[0].questcond = true;
                cout<<"\n   ->That should be enough meat for Grognak's feast.\n";
            }
		}
    //Check for level up
    lvlup();
}//end of combat function

void compTalk()
{
    bool is_companion;
    string inp;

    do
    {
        //Show companion greeting based on relationship level;
        //Since there are only 3 greetings, safeguard against higher levels
        int rel=pc.comp.rel; if(rel>2) rel=2;
        cout<<pc.comp.greet[rel];
        //Dialogue options
        cout<<"(1) Talk\n(2) Trade\n(3) Change tactics\n(4) View details\n(5) Dismiss\n(6) Nevermind.\n";
        cin>>inp;
        if(inp=="1")
        {
            cout<<pc.comp.chat[pc.comp.rel];
        }//talk
        else if(inp=="2")
        {
            prln("You propose trading items with your companion.");
            cout<<"(1) Give item\n(2) Take item\n(3) Back\n";
            cin>>inp;
            if(inp=="1")
            {
                pc.showInv();
                cout<<"Give which item to "<<pc.comp.name<<"? (inventory slot #)\n";
                int num; num=check_int();;
                if(pc.inv[num].name!="empty")
                {
                        if(pc.comp.inv.name!="empty" && pc.comp.inv.name != "") {pickup(pc.comp.inv); cout<<"You take the item they're holding first.\n";}
                        pc.comp.inv=pc.inv[num]; pc.carrywt-=pc.inv[num].wt; pc.inv[num]=eq[0];
                        cout<<"\nYou hand your companion the item.\n";
                        //If item is character's preferred gift
                        if(pc.comp.inv.id==pc.comp.gift){pc.comp.inv=eq[0]; cout<<pc.comp.accept; pc.comp.rel++;}
                        //If item is a type of equipment
                        if(pc.comp.inv.type=="weapon"&&pc.comp.weap.name=="empty")
                        {
                            pc.comp.equip(pc.comp.inv);
                            pc.comp.inv=eq[0];
                        }
                        if(pc.comp.inv.type=="accessory"&&pc.comp.acc.name=="empty")
                        {
                            pc.comp.equip(pc.comp.inv);
                            pc.comp.inv=eq[0];
                        }
                        if(pc.comp.inv.type=="armor"&&pc.comp.arm.name=="empty")
                        {
                            pc.comp.equip(pc.comp.inv);
                            pc.comp.inv=eq[0];
                        }
                    }
                }//End of Trade -> Give
                //Trade -> Take
                else if(inp=="2")
                {
                    cout<<"Take which item?\n(1) Weapon\n(2) Armor\n(3) Accessory\n(4) Inventory\n(5) Return\n";
                    cin>>pc.inp;
                    if(pc.inp=="1")
                    {
                            if(pc.comp.weap.name != "empty" && pc.comp.weap.name != "") {pickup(pc.comp.weap); pc.comp.weap=eq[0];
                            pc.comp.atk-=pc.comp.weap.atkb;
                            pc.comp.def-=pc.comp.weap.defb;
                            pc.comp.str-=pc.comp.weap.strb;
                            pc.comp.dex-=pc.comp.weap.dxb;
                            pc.comp.intl-=pc.comp.weap.intb;
                            pc.comp.lck-=pc.comp.weap.lckb;}
                            else prln("There's nothing there to take!");
                    }//End of Trade -> Take -> Weapon
                    if(pc.inp=="2")
                    {
                            if(pc.comp.arm.name != "empty" && pc.comp.arm.name != "") {pickup(pc.comp.arm); pc.comp.arm=eq[0];
                            pc.comp.atk-=pc.comp.arm.atkb;
                            pc.comp.def-=pc.comp.arm.defb;
                            pc.comp.str-=pc.comp.arm.strb;
                            pc.comp.dex-=pc.comp.arm.dxb;
                            pc.comp.intl-=pc.comp.arm.intb;
                            pc.comp.lck-=pc.comp.arm.lckb;}
                            else prln("There's nothing there to take!");
                    }
                    if(pc.inp=="3")
                    {
                            if(pc.comp.acc.name != "empty" && pc.comp.acc.name != "") {pickup(pc.comp.acc); pc.comp.acc=eq[0];
                            pc.comp.atk-=pc.comp.acc.atkb;
                            pc.comp.def-=pc.comp.acc.defb;
                            pc.comp.str-=pc.comp.acc.strb;
                            pc.comp.dex-=pc.comp.acc.dxb;
                            pc.comp.intl-=pc.comp.acc.intb;
                            pc.comp.lck-=pc.comp.acc.lckb;}
                            else prln("There's nothing there to take!");
                    }
                    else if(pc.inp=="4")
                    {
                        if(pc.comp.inv.name != "empty" && pc.comp.inv.name != "")
                            {pickup(pc.comp.inv);
                             pc.comp.inv=eq[0];}
                        else prln("There's nothing there to take!");
                    }
                    else if(pc.inp=="5")
                        break;
                }//End of Trade -> Take
                else if(pc.inp=="3")
                    break;
            }//End of Trade
            else if(inp=="3")
            {
                cout<<"\nYour companion's battle attitude is currently "<<pc.comp.ai<<".\n";
                cout<<"Should it be...\n(1) Aggressive\n(2) Cunning\n(3) Flexible\n(4) Carefree\n";
                cin>>pc.inp;
                if(pc.inp=="1") pc.comp.ai="agressive";
                else if(pc.inp=="2") pc.comp.ai="cunning";
                else if(pc.inp=="3") pc.comp.ai="flexible";
                else if(pc.inp=="4") pc.comp.ai="carefree";

                pc.comp.initializeAI();
            }
            else if(inp=="4")
            {
                pc.comp.stats();
            }
            else if(inp=="5")
            {
                cout<<"\nYou bid your companion farewell on their travels, as you go your separate ways.\n";
                pc.comp=empty1;

                for(int x = 0; x<3; x++)
                    if(loc[pc.area.x][pc.area.y].npcs[x].id = 0)
                        {loc[pc.area.x][pc.area.y].npcs[x] = comp_npc[pc.comp.npc_id];
                         pc.area.npcs[x] = comp_npc[pc.comp.npc_id];}
            }
            else if(inp=="6")
                cout<<"'Alright. We'll talk later, then.'\n";
        }while(inp!="6");
}

void theSick()
{
    int pot=rand()%3;
    string pots[3]={"health", "mana", "antidote"};
    bool has=0;

    if(!m_data.sick_nearby) sick.initialize(0);

    if(m_data.sick_nearby==true)
    {
        if(m_data.sick_potion_needed=="health") pot=0;
        if(m_data.sick_potion_needed=="mana") pot=1;
        if(m_data.sick_potion_needed=="antidote") pot=2;
    }

    switch(pot)
    {
        case 0: {if(pc.hpot>0) has=1;} break;
        case 1: {if(pc.mpot>0) has=1;} break;
        case 2: {if(pc.apot>0) has=1;} break;
    }

    cout<<"\nYou see "<<aoran(sick.age, 0)<<sick.showName("agen")<<" hunched over by the side of the road, pale and sickly.\n'Traveler,' "<<sick.pers<<" groans, 'Please, I need medicine..'\n'Do you have a single "<<pots[pot]<<" potion you can spare?\n";
    cout<<"(1) yes\n(2) no\n";
    cin>>pc.inp;

    if(pc.inp=="1"&&has==1)
    {
        switch(pot)
        {
            case 0: pc.hpot--; break;
            case 1: pc.mpot--; break;
            case 2: pc.apot--; break;
        }
        cout<<"\nYou hand the "<<sick.showName("agemf")<<" a potion and "<<sick.pers<<" quaffs it readily.\nSoon the color returns to "<<sick.pos<<" face and she straightens up.\n";
        cout<<"'Thank you, adventurer! I feel as good as new. Here's something for your trouble.\n\n";
        int xpg=rand()%pc.intl+4;
        int gpg=rand()%pc.lck+ pc.karma + 10;
        cout<<"Mini-quest Complete!!\nGained "<<xpg<<" XP\nGained "<<gpg<<" Gold\n";
        if(game_settings.sound) mciSendString(sdGlimmer.c_str(), NULL, 0, NULL);
        pc.xp+=xpg;
        pc.gp+=gpg;
        m_data.sick_nearby=false;
        m_data.sick_potion_needed="null";
        pc.karma++;
        if(!trophy_list[trophyGetID("potion")].unlock&&rollfor(1,100)) trophyGet("potion", 0);
    }
    else
    {
        cout<<"\n"<<sick.pers_up<<" gives a weak nod.\n'Thanks anyway, stranger.'\n";
        m_data.sick_nearby=true;
        m_data.sick_potion_needed=pots[pot];
        return;
    }
}

void theNeedful()
{
    int item = roll(75);

    if(m_data.needful_nearby == true) item = m_data.needful_item_id;

    else needful.initialize(0);

    int has = searchinv( item );


    cout<<"\nYou see "<<aoran(needful.adj, 0)<<needful.showName("adjn")<<" sitting,\nhead down, elbows on "<<needful.pos<<" knees.\n"<<needful.pers_up<<" glances up at your approach.\n";
    cout<<"'Oh! A stranger. Hey, friend, you wouldn't happen to have "<<aoran(eq[item].mat, 0)<<eq[item].showName()<<", would you?\n";
    cout<<"(1) yes\n(2) no\n";
    cin>>pc.inp;

    if( pc.inp == "1" && has != 0 )
    {
        cout<<"You hand the item over, smiling as the " << needful.mf << "'s expression brightens.\n'Thank you!' "<<needful.pers<<"cries, shaking your hand vigorously before striding away.\n";
        pc.inv[ has ] = eq[0];
        int xpg = roll(20 + pc.intl + 10);
        int gpg = roll(200 + pc.lck + pc.karma + 20);
        cout<<"\nMini-quest Complete!!\nGained "<<xpg<<"XP and "<<gpg<<" Gold\n";
        if(game_settings.sound) mciSendString(sdGlimmer.c_str(), NULL, 0, NULL);
        pc.xp += xpg;
        pc.gp += gpg;
        m_data.needful_nearby = false;
        m_data.needful_item_id = 0;
        pc.karma += 2;
        if(!trophy_list[trophyGetID("crate")].unlock&&rollfor(pc.karma,1000)) trophyGet("crate", 0);
    }
    else
    {
        cout << "The "<< needful.mf <<" nods mournfully. 'I see. Alright then.'\n";
        m_data.needful_nearby = true;
        m_data.needful_item_id = item;
        return;
    }
}

void talk()
{
	/*quest advancement note:
	since -area- npcs' quest levels are
	global, it's definitely possible to
	set/raise them from within a
	different function.
	For example, in the combat function,
	if you win, check enemy id,
	if it was a slime, check quest
	level on the guy who wanted you
	to kill a slime; if he's at the level
	where he asked you to do it,
	advance the quest or set the trigger.
	*/
	cout<<"Who do you want to talk to?\n";
	cin>>pc.inp;
	if(pc.inp=="to") cin>>pc.inp;

	pc.inp = strCase(pc.inp, "upper");

	bool pres=false;
	bool qcond;
	npc cur; //A temporary copy of the specified NPC
	int num;
	int x2;

	//Check input against local NPCs
	for(int x=0; x<3;x++)
	{
		if(pc.inp==pc.area.npcs[x].name)
		{
			pres=true;
			cur=pc.area.npcs[x];
		}
	}
	//If player entered companion's name
	if(pc.inp==pc.comp.name) {compTalk(); return;}

	//If player entered the name of a nearby side quest character
	if(m_data.lost_in_party&&pc.inp==lost.n) {lost.talk(); return;}
	if(m_data.sick_nearby&&pc.inp==sick.n) {theSick(); return;}
	if(m_data.needful_nearby&&pc.inp==lost.n) {theNeedful(); return;}

	//Enemy object chats
	if (pc.inp == pc.area.foe.name) {
        if (pc.area.foe.name == "Villager"){
            prln("'Hello, stranger.'");
        }
        else if (pc.area.foe.name == "Town Guard" || pc.area.foe.name == "Knight"){
            prln("'Move along, citizen.'");
        }
        else if (pc.area.foe.name == "Paladin") {
            if (pc.clas == "Paladin") { prln("Hail, Paladin.");}
            else prln("Move along, citizen.");
        }
        else prln("That can't talk right now!");
        return;
	}

    if(pres==false)
        cout<<"That person isn't here right now!\n";
		else
		{
		    //Show the character's greeting
			cout<<"\n"<<cur.greet;

			//If quest is incomplete, show dialogue line
			//relating to current quest level
			if(cur.q.lvl > 0 && cur.q.comp == false)
			{
				cout<<cur.qline[cur.q.lvl];

				//Unique greeting reactions here
				if(cur.id==2) //Farmer gives a seed bag
				if(cur.q.lvl==5)
					{
						int x=searchinv(298);
						if(x==0)
							pickup(eq[298]);
					}
			}
			else if(cur.q.comp==true)
				cout<<cur.qcomps;
			do
			{
            //check for quest item
			x2=searchinv(cur.q.req[cur.q.lvl]);
			if(x2!=0&&cur.q.comp==false) qcond=true;

			//offer dialogue options relating to quest lvl
				cout<<"\n"<<cur.dopt[cur.q.lvl];
				if(tutorial||cur.q.comp) cout<<"(4) Reset dialogue\n";

				num = check_int();

                cout<<"\n";
				switch(num)
				{
					case 1://quest dialogue
					{
						//if player has quest item or if quest hasn't started yet, advance quest
						if(cur.questcond==true) qcond=true;
						if(qcond==true||cur.q.lvl==0)
						{
							pc.carrywt-=pc.inv[x2].wt;
							pc.inv[x2]=eq[0];
							cur.q.lvl++;
						}
						//if quest incomplete, show next quest dialogue
						if(cur.q.comp==false)
						{
							cout<<cur.qline[cur.q.lvl];

							// Effects by quest step
							//
							//--------------------------------
							if(cur.name=="Archmage"&&cur.q.lvl==6)
                            {
                                pc.area.foe=elist[21];
                                pc.area.foe.alive=true;
                                while (pc.area.foe.alive) combat();
                                cout<<"\n(Enter '1' to continue)";
                            }
                            if(cur.name=="Suni")
                            {
                                if (cur.q.lvl==2){
                                    if( !searchinv(207) ) pickup(eq[207]); }//sacred torch
                                if (cur.q.lvl == 3) {if ( !searchinv(275) ) pickup( eq[275] ); }
                            }
                            if(cur.name=="Shaman"&&cur.q.lvl==3)
                            {
                                int f_torch=searchinv(208);
                                if(f_torch==0)
                                {
                                    cout<<"\n(The shaman hands you a foul torch.)\n\n";
                                    pickup(eq[208]);
                                }
                            }
                            if(cur.name=="Viri"&&cur.q.lvl==2)
                            {
                                cout<<"\n(You accept the smooth stone from Brother Viri.)\n";
                                pickup(eq[209]);
                            }
                            if(cur.name=="Gareth"&&cur.q.lvl==cur.q.end)
                            {
                                cout<<"\n(Gareth hands you a Paladin's Crest.)\n";
                                pickup(eq[295]);
                            }
                            if(cur.name=="Valencia"&&cur.q.lvl==1)
                            {
                                cout<<"\n(Valencia hands you the vial of poison.)\n";
                                pickup(eq[288]);
                            }
                            if(cur.name=="Valencia"&&cur.q.lvl==cur.q.end)
                            {
                                cout<<"\n(Valencia hands you a slayer coin to give to Hakon.)\n";
                                pickup(eq[210]);
                            }
                            if(cur.name=="Elder"&&cur.q.lvl==2)
                            {
                                if( !searchinv( getItemID("silver", "flask") ) ) {
                                cout<<"\n(The Elder hands you a sparkling silver flask inscribed with strange runes.)\n";
                                pickup(eq[292]);
                                }
                            }
						}
						else//show npc dialogue for after you've done their quest
						{
							cout<<cur.qcomps;

							//check by npc id and quest level for unique events, like getting a quest item or unlocking an area
							//Farmer
							if(cur.id==2)
							{
								int x=searchinv(298);
								if(x==0){cout<<"Here's a free bag of seed, for your troubles.\n"; pickup(eq[298]);}
							}
							//Sera
                            if(cur.id==3)
							{
								if(pc.apot==0)
								{cout<<"'You're out of antivenom? Here, you\nshould always have at least one. Just in case.'\n"; pc.apot++; cout<<"Received an Antidote.\n";}
							}
							//Aria
							if(cur.name == "Aria")
							{
							    if(!searchinv(getItemID("wooden", "flute")))
                                {
                                    prln("'Oh! I forgot, I had an extra...'\n\nShe reaches into a fold of her dress and hands you a flute.");
                                    pickup(eq[getItemID("wooden", "flute")]);
                                }
							}
							//If character asks Luz to join them after completing her quest
							if(cur.name == "Luz" && cur.q.comp)
                            {
                                if (pc.comp.name != "empty"){ prln("She looks at you sideways.\n'You've already got someone you're traveling with, right?'\nShe yanks an arrow out of her target.\n'Three's a crowd, you know.'"); return; }

                                prln("She looks at you strangely, then looks away, out over the horizon.");
                                prln("The two of you can see out over the Elven village below,\nand several silent minutes pass as you watch the placid villagers\ngoing about their business.");
                                prln("Finally she looks back at you, a steely gleam in her eye.\n\n'You know what? Sure. Let's go.'\n\nAs you turn to leave, she grabs your shoulder.\n'Just so you know, this doesn't mean I trust you.\nOr even really know you, for that matter.'\n\nYou shrug, not really knowing how to respond; the two of you set out together.");

                                prln("  -> Luz joins your party!");

                                pc.comp = luz;
                                pc.comp.initializeInv();
                                pc.comp.initializeAI();

                                pc.area.npcs[0] = nlist[0];
                                loc[9][10].npcs[0] = nlist[0];

                                //Set elven patriarch's quest level if at the appropriate step
                                if(loc[8][10].npcs[0].q.lvl == 3)
                                {
                                    loc[8][10].npcs[0].questcond = true;
                                    prln("  ->You can tell the Elven Patriarch you were successful");
                                }
                            }
                            //Grognak
                            if(cur.name == "Grognak" && cur.q.comp)
                            {
                                if (pc.comp.name != "empty"){ prln("Grognak laughs, and you get a strong waft of blood and...feathers?.\n'You have someone already, yes? They keep you safe.'"); return; }

                                prln("Grognak shrugs his huge shoulders. 'Tribe fed now, not go hungry without Grognak. Why not'?");

                                prln("  -> Grognak joins your party!");

                                pc.comp = grognak;
                                pc.comp.initializeInv();
                                pc.comp.initializeAI();

                                pc.area.npcs[0] = nlist[0];
                                loc[1][8].npcs[0] = nlist[0];
                            }
                            //D'tuum
                            if(cur.name == "D'tuum")
                            {
                                prln("The Dwarf looks you over.\n'Y'know, with all that diggin', I'd wager the Mining Guild could use ye.\nHere, I'll talk to the guards, tell 'em how ye helped me - they should let you through.\nTell the guildmaster I sent yeh.\n");

                                gameMsg("You can now enter the Underground City to the east.");

                                m_data.dwarven_city = true;
                                loc[1][4].east = true;
                                if (pc.area.name == "Dwarven Mine") pc.area.east = true;
                            }

							if(!tutorial&&!trophy_list[cur.q.id+1].unlock&&rollfor(1,100)) trophyGet("null", cur.q.id+1);

						}
						qcond=false;
						cur.questcond=false;

						//if, during conversation, quest is completed(and wasn't already)
						if(cur.q.end==cur.q.lvl&&cur.q.comp==false)
						{
							cout<<cur.qcomps;
							cur.q.comp=true;

							//quest rewards if not in tutorial mode
							if(!tutorial){
							pc.xp+=cur.q.xpr;
							pc.gp+=cur.q.gpr;
							cout<<"\nQUEST COMPLETE\nEarned "<<cur.q.xpr<<" XP, "<<cur.q.gpr<<" GP\n";
							if(eq[cur.q.itr].name!="empty"){cout<<"Got a "<<eq[cur.q.itr].name<<".\n"; pickup(eq[cur.q.itr]);};
							}

							//switch to enable unique quest outcomes, like opening an area or changing stats
							switch(cur.id)
							{
								case 1:
								break;
								case 6: {cout<<" ->You can now cross the river to the east.\n"; loc[2][6].east=true; pc.area.east=true; m_data.ferry=true;}
								break;
								case 8: {cout<<" ->You can now cross the river to the north.\n"; loc[5][9].north=true; pc.area.north=true; m_data.bridge=true;}
								break;
								case 10:
								    {pc.clas="Paladin";
								    pc.hpmax+=1.2*pc.hpmax; pc.hp=pc.hpmax;
								    pc.str+=3; pc.dex+=0; pc.intl+=2; pc.lck+=1;
								    cout<<"\n  -> Your class has been upgraded to Paladin.\n";}
                                break;
                                case 12:
                                    {
                                        pc.clas="Assassin";
                                        pc.hpmax+=1.1*pc.hpmax; pc.hp=pc.hpmax;
                                        pc.str+=1; pc.dex+=3; pc.lck+=2;
                                        cout<<"\n  -> Your class has been upgraded to Assassin.\n";
                                    }
                                break;
                                case 14:
                                    {
                                        pc.clas="Archon";
                                        pc.hpmax+=1.1*pc.hpmax; pc.hp=pc.hpmax;
                                        pc.str+=0; pc.dex+=0; pc.intl+=6; pc.lck+=0;
                                        cout<<"\n  -> Your class has been upgraded to Archon.\n";
                                    }
                                break;
                                case 16:
                                    {
                                        pc.clas="Avatar";
                                        pc.hpmax+=1.3*pc.hpmax; pc.hp=pc.hpmax;
                                        pc.str+=0; pc.dex+=0; pc.intl+=2; pc.lck+=4;
                                        cout<<"\n  -> Your class has been upgraded to Avatar.\n";
                                    }
                                break;
							}
						}//end of if quest was completed
					}//end of if player chose dialogue option 1
					break;
					case 2:
					    {
					        if(cur.name=="Aria"&&cur.q.comp) aria_sings();
					        else cout<<cur.info[cur.q.lvl];
					    }

					break;
					case 3:
						cout<<cur.bye;
					break;
					case 4:
                        {
                            cout<<"\nRestart this character's quest and dialogue?\n(1) Yes\n(2) No\n";
                            num=check_int();
                            if(num==1) {cur.q.lvl=0; cur.q.comp=false; cout<<"This character has been reset.\n";}
                        }
                    break;
					default:
						cout<<cur.def;
					break;
				}
			}while(num!=3&&num!=4);

			//return temp npc values to area npc (to save quest level)
			for(int x=0;x<3;x++)
			{
				if(pc.area.npcs[x].name==cur.name)
				{
					pc.area.npcs[x].q.lvl=cur.q.lvl;
					//local area npcs, temp
					pc.area.npcs[x]=cur;
					//area index npcs, static
					if(tutorial) tut[pc.area.x][pc.area.y].npcs[x]=cur;
					else loc[pc.area.x][pc.area.y].npcs[x]=cur;
				}
			}

			//set global quest level for questlog
			if(!tutorial) qlog[cur.q.id]=cur.q;

			if(!tutorial) cout<<"Debug: quest '"<<qlog[cur.q.id].name<<"' level set to: "<<qlog[cur.q.id].lvl<<"\n";
		}
	//and that's it! npcs and quests with up to 10 steps are now frameworked! Conversations not relating to quests are impossible! Quest items are allowed! wooooooooo~
}

void show_map()
{
    if(game_settings.sound) mciSendString(sdPaper.c_str(), NULL, 0, NULL);
	int x, y;
	int pctexp=0;

	if ( pc.area.world == "Overworld" ) {
	for(y=1; y<11;y++)
		for(x=1; x<11; x++)
		{
			if(loc[x][y].visits==0&&loc[x][y].name!="River"&&pc.inp!="maptest")
				cout<<"|__";
			else if(loc[x][y].id==pc.area.id)
				{ colSet(COL_WHITE, "|"); colSet(game_settings.pcol_1, game_settings.psym_1); colSet(game_settings.pcol_2, game_settings.psym_2); }
			else
			{
			    //Legacy code displaying map tiles as ASCII symbols -v
			    //if(loc[x][y].map_color1==0&&loc[x][y].map_color2==0)
				//cout<<loc[x][y].key;

				if (x == pc.area.x + 1 && y == pc.area.y) colSet(COL_WHITE, "|"); else cout<<"|";
				colSet(loc[x][y].map_color1, "_");
				colSet(loc[x][y].map_color2, "_");

				pctexp++;
			}
			if(x > 9) cout << "|\n"; //------------------------------\n";
		}
	} // If overworld
		cout<<"Explored: "<<pctexp<<"%\n";
		cout<<"("<<pc.area.x<<", "<<pc.area.y<<"): "; colSet(240, pc.area.name); cout<<"\n";
}

void legend()
{
	cout<<"[__]: Unexplored\nX: player\nV: Village\nF: Forest\nf: Sparse Forest\nfp: Forest Path\nG: Grassland\nG-, G|, G/: Grassland Path\nG~: Grassland Riverbend\n~~: River\nM: Mountain\n~F: Ferry\n||: Bridge\n\n";
}

void offshore()
{

	cout<<"(1) Deposit\n(2) Withdrawal\n";
	cin>>pc.inp;
	if(pc.inp=="1")
	{
		cout<<"How much?\n";
		int num;
		num=check_int();;
		if(num<=pc.gp)
		{
			pc.gp-=num;
			bank+=num;
			cout<<"Your transaction has been approved.\n";
		}
		else
			cout<<"You don't have enough!\n";
	}
	else if(pc.inp=="2")
	{
		cout<<"How much?\n";
		int num;
		num=check_int();;
		if(num<=bank)
		{
			pc.gp+=num;
			bank-=num;
			cout<<"Your transaction has been approved.\n";
		}
		else
			cout<<"You don't have enough!\n";
	}
}

equipment itemroll(int start, int range)
{
	int item = rollfor(start, range);
	equipment itemp = eq[item];
    if(itemp.name == "book")
                {
                    int y=0;
                    if(itemp.mat == "skill")
                    {
                        string skill[10]={"woodcutting", "firemaking", "mining", "fishing", "cooking", "carving", "smithing", "sewing", "enchanting", "hunting"};
                        y=rand()%10;
                        itemp.ench=skill[y];
                    }
                    else if(itemp.mat=="spell")
                    {
                        y=rand()%68;
                        itemp.ench=splist[y].name;
                    }
                    itemp.matb=y;
                }
    if(itemp.name=="scroll")
    {
        int y=roll(68);
        itemp.ench=splist[y].name;
        itemp.matb=y;
    }
    if(itemp.name=="amulet")
    {
        string gem[6]={"sapphire", "ruby", "emerald", "crystal", "amethyst", "diamond"};
        string elem[6]={"water", "fire", "earth", "wind", "lighting", "ice"};
        //int, str, def, dxt, lck, atk
        int type=rand()%6;
        itemp.mat+=" "+gem[type];
        itemp.ench=elem[type];
        itemp.enchb+=itemp.matb;
        itemp.defb=0;
        switch(type)
        {
            case 0: itemp.intb += itemp.matb; break;
            case 1: itemp.strb += itemp.matb; break;
            case 2: itemp.defb += itemp.matb; break;
            case 3: itemp.dxb += itemp.matb; break;
            case 4: itemp.lckb += itemp.matb; break;
            case 5: itemp.atkb += itemp.matb; break;
        }
    }
    if(itemp.type=="container"&&itemp.ench=="none")
                {
                    string pot[4]={"strength", "dexterity", "intellect", "luck"};
                    int ptype=rand()%5;
                    if(ptype<4)
                    {
                        string pname=pot[ptype]+"-potion";
                        itemp.ench=pname;
                        itemp.subt="potion";
                    }
                    else itemp.ench="none";
                }
	return itemp;
}

void chestroll()
{
    int chance = 10 + pc.lck;
    if ( chance > 25 ) chance = 20;
	if ( rollfor( (10 + pc.lck), 100) && !pc.area.water && pc.area.chest == 0 )
	{
		pc.area.chest = 1;
		loc[pc.area.x][pc.area.y].chest = 1;
		cout << "\nYou found an old treasure chest!\n";
		if(game_settings.sound) mciSendString("NULL", NULL, 0, NULL);
	}
}

void openchest()
{
    //This function rolls for chest loot
    //and reassigns the area chest variable

	equipment itemp = eq[0];

	if(pc.area.chest == 1)
	{
		int chance=roll(20); //Roll for chance of loot - items, gold, or potions (or nothing)
		switch(chance)
		{
			case 1 ... 5: //Item
			{
				while (itemp.name == "empty") itemp = itemroll(1, 150);
				cout << "\nYou found " << aoran(itemp.showName(), false) << itemp.showName() << ". Keep? (y/n)\n";
				cin>>pc.inp;
				if(inpAffirm(pc.inp)) { pickup(itemp); }
				else
				{
				    prln("You leave the item where it is.");
                    pc.area.loot = itemp;
				}
			}
			break;
			case 6 ... 10: //Gold
			{
				int gp = roll(30 + pc.lck);
				cout << "You found " << gp << " gold coins!\n";
				if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\coins.mp3", NULL, 0, NULL);
				pc.gp+=gp;
			}
			break;
			case 11 ... 15: //Potion
			{
				int pot = roll(3);
				string s_pot[4] = {"null", "health", "mana", "antidote"};
				cout << "You found " << aoran(s_pot[pot], false) << s_pot[pot] << " potion!\n";
				if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\chakram.wav", NULL, 0, NULL);
				switch(pot)
				{
					case 1: {pc.hpot++;} break;
					case 2: {pc.mpot++;} break;
					case 3: {pc.apot++;} break;
				}
			}
			break;
			case 16 ... 20:
			{
				cout<<"Nothing but cobwebs.\n";
			}
			break;
		}
		pc.area.chest=2;
		loc[pc.area.x][pc.area.y].chest=2;

		if(!trophy_list[trophyGetID("chest")].unlock&&rollfor(1,100)) trophyGet("chest", 0);
	}
	else if(pc.area.chest==2)
		cout<<"This chest has already been looted by some vagabond.\n";
	else
		cout<<"There's no chest here!\n";
}

void wc()
{
	if(pc.axe==true)
	{
		if(pc.area.wood==true)
		{
			int wchance=rand()%15+1;
			if(pc.wclvl+(pc.lck/2)>=wchance)
			{
				cout<<"\nYou manage to chop a bit of wood.\n";
				if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\chop.wav", NULL, 0, NULL);
				pickup(eq[49]);
				if(pc.wclvl < 20) pc.wclvl++;
				if(!trophy_list[trophyGetID("cut logs")].unlock&&rollfor(1,100)) trophyGet("cut logs", 0);
			}
			else{
                if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\chop3.wav", NULL, 0, NULL);
				string miss[4] = {
                    "You swing your axe at the tree but discover\nyou are using the wrong end.",
                    "You swing your axe at the tree but\nmiss horribly, burying the blade in the dirt.",
                    "You swing your axe at the tree but it flies out of your hands.\nYou waste a few minutes finding it in the ferns.",
                    "You try to cut some logs.\nThey crumble under the stroke of the axe, rotten."
				};

				cout << "\n" << miss[rand()%4] << "\n";
			}
		}
		else cout<<"There aren't any trees around here!\n";
	}
	else cout<<"You don't have an axe, and\nyour weapon would break if you tried\nto fell a tree with it.\n";
}

void forage()
{
        cout << "\nYou search the area for anything edible.\n";

	    if (pc.area.flora_types.size() > 0 ){
            int ftype = rand()%pc.area.flora_types.size();
            if( rollfor( pc.frglvl + pc.lck, 39 ) )
                { pc.area.loc_flora = pc.area.flora_types[ftype];
                cout << "\nYou found " << aoran(pc.area.loc_flora.name, false) << pc.area.loc_flora.name << " " << pc.area.loc_flora.idle << " nearby.\n"; }
            else
                { pc.area.loc_flora = plant[0];
                cout << "\nYou didn't find anything useful.\n"; }
                pc.frglvl++;
            }
        else
            { pc.area.loc_flora = plant[0];
            cout << "\nIt doesn't look like anything grows around here.\n";
            pc.frglvl++; }
}

void mine()
{
	if(pc.pick==true)
	{
		if(pc.area.mine==true)
		{
		    int msfx=rand()%3;
		    switch(msfx)
		    {
            case 0: if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\mine1.wav", NULL, 0, NULL);
                break;
            case 1: if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\mine2.wav", NULL, 0, NULL);
                break;
            case 2: if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\mine3.wav", NULL, 0, NULL);
                break;
		    }

				int ore;
				int och=rand()%100+1;
				//essentially roll for (pc.mlvl*5) out of 100
				//
				//
				if(och<=pc.mlvl*5){
				switch(och)
				{
					case 1 ... 20://iron is level 1-4
						ore=48;
					break;
					case 21 ... 40://coal levels 4-8
						ore=45;
					break;
					case 41 ... 45://silver is level 8-9
						ore=54;
					break;
					case 46 ... 50://gold level 9-10
						ore=55;
					break;
					case 51 ... 60://mithril is level 10-12
					{
						if(pc.area.id==10) ore=44; //Mithril is only mined in one location
						else ore=48;
					}
					break;
					case 61 ... 70://crystal at level 12-14
						ore=46;
					break;
					case 71 ... 75://diamond at level 14-15
						ore=47;
					break;
					case 76 ... 80://amethyst level 15-16
						ore = 53;
					break;
					case 81 ... 85://emerald level 16-17
						ore=50;
					break;
					case 86 ... 90://sapphire level 17-18
						ore=51;
					break;
					case 91 ... 95://ruby level 18-19
						ore=52;
					break;
					case 96 ... 100://a second chance at iron unlocks at level 20 (???)
						ore=48;
					break;
				}

				cout<<"You manage to mine some "<<eq[ore].name<<".\n";
				pickup(eq[ore]);
				if (pc.mlvl < 20) pc.mlvl++;
				}
				else
				{
					int x=roll(5);
					switch(x)
					{
					case 1:
					{
                        cout<<"You see a mineral you can't identify.\nYou attempt to mine it, but come away with\nregular stone.\n";
                        pickup(eq[66]);
                        if(pc.mlvl < 20) pc.mlvl++;
					}
					break;
					default:
					cout<<"You see a mineral you can't identify.\nYou attempt to mine it, unsuccessfully.\n";
					break;
					}
				}
				if(!trophy_list[trophyGetID("pickaxe")].unlock&&rollfor(1,100)) trophyGet("pickaxe", 0);
		}
		else cout<<"There aren't any mines around here!\n";
	}
	else cout<<"You don't have a pickaxe, and\nyour weapon would snap if you tried\nto break rocks with it.\n";
}

void fish()
{
	string fish_types[6]={"greengill", "catfish", "salmon", "trout", "river bass", "pike"};
	int wt[6] = {1, 2, 3, 4, 5, 6};
	bool roe_chance = roll20(1);

	//Fishing errors...
	if( !pc.frod ) { cout << "You don't have a fishing pole...\n"; return; }
    if( !pc.bait ) { cout << "You're not going to catch any fish without bait!\n"; return; }
    if( !pc.area.water ) { cout << "There isn't any water around here!\n"; return; }

    if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\fishing.wav", NULL, 0, NULL);

    prln("You cast your line...");
    int wait = roll(5);
    for (int i = 0; i < wait; ++i) { prln("..."); Sleep(1000); }

    //If fishing success
    if( pc.frod * ( pc.flvl + ( pc.lck / 2 ) ) >= roll(20) )
    {
        cout<<"You manage to catch a fish!\n";
        //Find empty inventory slot
        int x = searchinv(0);
        //Create random fish (type, weight)
        int fch = roll(5);
        wt[ fch ] = roll( wt[ fch ] * 2 );
        pc.inv[x] = eq[39]; //raw fish
        pc.inv[x].ench = fish_types[ fch ]; //add fish name to object
        pc.inv[x].wt = wt[ fch ]; //set weight
        //Add fishing experience
        if (pc.flvl < 20) pc.flvl++;
        pc.xp++;

        if( rollfor( pc.lck, 100 ) ) prln("What luck! You got your bait back!");
            else pc.bait--;

        cout << "It's a " << fish_types[ fch ] << "...and it weighs " << wt[ fch ] << " pounds!\n";
        if( wt[ fch ] >= 10 )
            cout<<"Wow! A real lunker!\n";
                if( roe_chance )
                {
                    pickup(eq[127]);
                    cout<<"\nWhat's this? You found some fish eggs, too.\n";
                }
                if(!trophy_list[trophyGetID("trout")].unlock&&rollfor(1,100)) trophyGet("trout", 0);
			}
			else
				cout<<"You attempt to catch a fish.....\nNot even a nibble.\n";
}

void carve()
{
    string crskl[13]=
	{
	    "null",
		"club",
		"staff",
		"small shield",
		"charm",
		"vial",
		"flute"
		"shortbow",
		"elemental staff",
		"arrows",
		"longbow",
	};
	int num;
	int x = 0;
	if(pc.chis == true)
	{
	    cout << "\nCarving skill: " << pc.clvl <<"\n";
	    cout << "\nItems available: \n";
		for(int ct = 1; ct < 10; ct++)
			if(ct<=pc.clvl/3||ct==1)
				cout << "[" << ct << "] --- " << crskl[ct] << "\n";

		cout<<"\nEnter the number of the item you want to make.\n";
		num = check_int();
		if( num > pc.clvl / 3 && num != 1 )
		{
			cout<<"You're not skilled enough to carve that!\n";
			return;
		}
        //Cycle through inventory for wood
		do{x++;}while(pc.inv[x].name!="wood"&&x<11);
		if(x>10)cout<<"You don't have the required materials.\n";
		//If player has wood
		else
		{
		    //Carving chance is luck+skill out of 10
			int chance=rand()%10+1;
			if(chance<=(pc.clvl+pc.lck)-num)
			{

            //Remove material
            pc.inv[x]=eq[0];

			//Add carving XP
			if (pc.clvl < 20) pc.clvl++;
			pc.xp++;

			//set switch for aria's quest
				if(loc[7][10].npcs[0].q.comp==false&&pc.clvl>3)
				{
					loc[7][10].npcs[0].questcond=true;
					cout<<"Your carving level is now high enough\nthat you could probably repair a flute.\n";
				}
            if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\chop2.wav", NULL, 0, NULL);
			cout<<"You chisel away at the wood until\nyou've carved the " <<crskl[num]<<".\n";

		switch(num)
		{
			case 1: pickup(eq[2]); break;
			case 2: pickup(eq[18]); break;
			case 3: pickup(eq[22]); break;
			case 4: pickup(eq[getItemID("wooden", "vial")]); break;
			case 5: pickup(eq[23]); break;
			case 6: pickup(eq[56]); break;
			case 7:
			{
				string elem[7]={"null", "wind", "water", "earth", "fire", "electric", "ice"};
				int elemnum;
				do
				{
					cout<<"With which element will you imbue this staff?\n";
					for(int x=1;x<7;x++)
						cout<<x<<": "<<elem[x]<<"\n";
					elemnum=check_int();
					if(elemnum>6)
						cout<<"Sorry, that's not an option.\n";
				}while(elemnum>6);
					int gem;
					string ngem, mat;
					//Initialize stat bonuses
					int atk=0;
					int def=0;
					int str=0;
					int dex=0;
					int intl=0;
					int lck=0;
					switch(elemnum)
					{
						case 1:
						{
							gem=searchinv(46);
							ngem="crystal.\n";
							mat="wind";
							dex=1;
						}
						break;
						case 2:
						{
							gem=searchinv(51);
							ngem="sapphire.\n";
							mat="water";
							intl=1;
						}
						break;
						case 3:
						{
							gem=searchinv(50);
							ngem="emerald.\n";
							mat="earth";
							def=1;
						}
						break;
						case 4:
						{
							gem=searchinv(52);
							ngem="ruby.\n";
							mat="fire";
							str=1;
						}
						break;
						case 5:
						{
							gem=searchinv(53);
							ngem="topaz.\n";
							mat="electric";
							lck=1;
						}
						break;
						case 6:
						{
							gem=searchinv(47);
							ngem="diamond.\n";
							mat="ice";
							atk=1;
						}
						break;
					}
					if(gem==0)
						cout<<"You don't have the right gemstone to make that.\n";
					else
					{
						pc.carrywt-=pc.inv[gem].wt;
						pc.inv[gem]=eq[0];
						int op=searchinv(0);
						pickup(eq[299]);
						pc.inv[op].mat=mat;
						pc.inv[op].desc+=ngem;
						pc.inv[op].name="staff";
						pc.inv[op].atkb+=atk;
						pc.inv[op].defb+=def;
						pc.inv[op].strb+=str;
						pc.inv[op].dxb+=dex;
						pc.inv[op].intb+=intl;
						pc.inv[op].lckb+=lck;
					}
			}
			break;
			case 8://arrows
				{pc.arrows+=10;}
			break;
			case 9:
				pickup(eq[57]);
			break;
			default: return;
		}
            if(!trophy_list[trophyGetID("staff")].unlock&&rollfor(1,100)) trophyGet("staff", 0);
		}
		else
		{
			cout<<"You drive the chisel too hard and the wood splinters!\n";
			if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\woodbreak.wav", NULL, 0, NULL);
			pc.carrywt-=pc.inv[x].wt;
			pc.inv[x]=eq[0];
		}
	}
	}
	else
		cout<<"You need a chisel to do that!\n";
}

void fire()
{
	int wood = searchinv( getItemID("none", "wood") );
	bool fmchance = rollfor(pc.fmlvl + pc.lck, 20);

	if( !pc.tbox ) {prln("You don't have a tinderbox, and you're\nnot skilled enough to light a fire without one."); return; }
	if( !wood ) { prln("You try to think of something to light on fire,\nbut wisely decide only wood will do."); return; }
	if( !fmchance ) {prln("You huff and puff but the fire never catches."); return; }

    cout << "\nYou manage to light a fire.\n";
    if (pc.fmlvl < 20)pc.fmlvl++;
    pc.xp++;
    pc.inv[wood] = eq[0];
    pc.area.fire = true;

    if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\campfire.wav from 0 to 750", NULL, 0, NULL);
    if(!trophy_list[trophyGetID("campfire")].unlock&&rollfor(1,100)) trophyGet("campfire", 0);
}

void dream()
{
    int t=3000+roll(2000);

    string wake[3]={
        "You awake with a start, drenched in cold sweat.",
        "You awake with an oddly warm feeling, stretching languidly,\na smile on your face.",
        "You awake with a strange, wistful feeling, and you can't quite remember...\nThere are tears on your cheeks."
    };
    string dream[7][3];

    dream[0][0]="You're deep underwater. Everything is brightly lit.\n";
    dream[0][1]="Through a forest of fronds you can see a floor of colorful pebbles.\n";
    dream[0][2]="You look up at a loud tapping noise. A giant goldfish peers at you from behind a wall of glass.\n";

    dream[1][0]="You're wandering through a dense forest in twilit dusk.\n";
    dream[1][1]="Motes of dust glimmer in the air, drifting around you.\n";
    dream[1][2]="You find a clearing, with a silver urn on a mossy boulder.\nYou reach out to touch it...\n";

    dream[2][0]="You're suspended in a limitless, inky void.\n";
    dream[2][1]="Echoes of strange, whispering voices skitter in the darkness.\n";
    dream[2][2]="'Look,' says one, 'It's waking up...'\n";

    dream[3][0]="You dream of a life in a distant valley.\nYou're a farmer, working the fields for your living.\n";
    dream[3][1]="You find a partner, and the village grows. Many seasons weather your skin.\n";
    dream[3][2]="At last, you lay in bed surrounded by your family.\nOut the window, the sun sets over your fields.\n";

    dream[4][0]="The golden grasses of the endless plain rustle around you, wind tousling your mane.\n";
    dream[4][1]="Your powerful nose finds the scent of prey, and the chase begins,\nmuscles rippling under golden fur as your paws pound the dirt.\n";
    dream[4][2]="Your jaws close around the gazelle's fragile neck.\n";

    dream[5][0]="You're chopping a tree.\n";
    dream[5][1]="You've gotten some logs, and are attempting to light them.\n";
    dream[5][2]="You manage to light a fire.\n";

    dream[6][0]="High winds ruffle your banded pinions. You scrutinize\nthe patchwork landscape far below with keen yellow eyes.\n";
    dream[6][1]="A massive, winged shadow circles the smoldering fire-mountain in the north.\n";
    dream[6][2]="Catching an updraft, you veer east, cold, dim, light fading behind you.\n";

    int n=roll(7)-1;
    int w=roll(3)-1;

    cout<<"You find the most comfortable spot you can and take\na short rest, using your pack as a pillow.\n\n";
    Sleep(2500);
    cout<<"You begin to dream...\n\n";

    for(int x=0; x<3;x++)
    {
        cout<<dream[n][x]<<"\n";
        Sleep(t);
    }
    cout<<wake[w]<<"\n\n";
    Sleep(2500);
    cout<<"You feel somewhat refreshed.\n";
}

void rest()
{
    if(pc.area.foe.alive) {cout<<"\nYou can't rest with enemies nearby!\n\n"; return;}

    dream();

    int gain=roll(12)+(pc.lvl/2);
    if(pc.area.fire||pc.area.name=="Village"||!pc.area.is_outside) gain*=2;

    pc.hp += gain;
    pc.mp += gain;

    if (pc.hp > pc.hpmax) pc.hp = pc.hpmax;
    if (pc.mp > pc.mpmax) pc.mp = pc.mpmax;

    pc.showHealth();
    pc.showMana();
}

void temper()
{
    //Search inventory for material
    int x = searchinvtype( "ore" );

    //Get item to temper
    pc.showInv();
    prln("Temper item from which inventory slot?");
    int num = pc.getInpn();

    //Create temporary vars for clarity
    string item_type = pc.inv[num].type;
    string item_mat = pc.inv[num].mat;
    string target_mat = pc.inv[x].mat;
    int sm_check = (pc.smlvl / 5);
    int sm_targ = pc.inv[x].matb;

    //Tempering escape sequences
    if( item_type!="weapon" && item_type!="armor" ) { prln("You can't improve that item!"); return; }
    if( item_mat != target_mat ) { prln("You need to use material of the same type to temper an item."); return; }
    if( sm_check < sm_targ ) { prln("Your skill in smithing is not high enough to refine that item."); return;}

    //Increase stats (only alter atk if object is a weapon)
    if( pc.inv[ num ].type == "weapon" ) pc.inv[ num ].atkb += pc.inv[ num ].matb;
    pc.inv[num].defb += pc.inv[ num ].matb;

    //Change object material to prevent further tempering
    pc.inv[num].mat = "tempered-" + pc.inv[ num ].mat;

    if( game_settings.sound ) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\chop2.wav", NULL, 0, NULL);

    prln("You reheat and refine the item, improving its durability.");
    //Remove material
    pc.carrywt -= pc.inv[ x ].wt;
    pc.inv[ x ] = eq[0];
}

void sew()
{
	string clothes[9]={"null", "cloth tunic", "leather tunic", "wizard hat", "leather small shield", "mage robes", "mage hood", "quiver", "sheath"};
	int item, num, first_mat;
    int second_mat = -1;

    //Check for sewing equipmentp-p-;
    if (!pc.need) { prln("You can't sew anything without a needle and thread!"); return; }

    //Display sewing options
    cout<<"Sewing Level: " << pc.swlvl << "\n";
    cout << "\n Level \t| Item\n";
    cout << "--------|---------------------\n";
    cout << "[1]: \t| " << clothes[1] << "\n";
    for(int it = 2; it < 8; it++)
        if( pc.swlvl >= ( (it * 2) - 2) ) cout << "[" << ( (it * 2) - 2) << "]: \t| " << clothes[it] << "\n";

	cout << "\nWhat item do you want to sew?\n";
	num = pc.getInpn();

	//Check sewing level
    if( pc.swlvl < num ) { prln("Your sewing skills aren't sharp enough to make that!"); return; }

    //Get item and material info
    switch(num)
    {
        case 1: {first_mat = searchinv(36); item = getItemID("cloth", "tunic");} break;
        case 2: {first_mat = searchinv(35); item = getItemID("leather", "tunic");} break;
        case 3: {first_mat = searchinv(36); item = getItemID("cloth", "wizard hat");} break;
        case 4: {first_mat = searchinv(35); item = getItemID("leather", "small shield");} break;
        case 5: {first_mat = searchinv(36); second_mat = pc.searchInvFrom("none", "wool", first_mat); item = getItemID("cloth", "mage robes");} break;
        case 6: {first_mat = searchinv(36); item = getItemID("cloth", "mage hood");} break;
        case 7: {first_mat = searchinv(35);} break;
        case 8: {first_mat = searchinv(35);} break;
    }

    //If player is missing either material
    if( !first_mat ) { prln("You lack the required items."); return; }
    if( second_mat == 0) { prln("You lack the required items."); return; }

    //Remove materials used
    pc.inv[ first_mat] = eq[0];
    if ( second_mat > 0 ) pc.inv[ second_mat ] = eq[0];

    //Add sewing XP
    if ( pc.swlvl < 20 ) pc.swlvl++;
    pc.xp++;

    prln("You diligently pull the needle and thread until you've woven the " + clothes[ num ] );

    //Trophy roll
    if(!trophy_list[trophyGetID("spool of thread")].unlock&&rollfor(1,100)) trophyGet("spool of thread", 0);

    //unlock notifications
    switch(pc.swlvl)
    {
        case 2: cout << "You are now proficient enough in sewing\nto make a leather tunic.\n"; break;
        case 4: cout << "You are now proficient enough in sewing\nto make a wizard hat.\n"; break;
        case 6: cout << "You are now proficient enough in sewing\nto make a leather shield.\n"; break;
        case 8: cout << "You are now proficient enough in sewing\nto make a set of mage robes.\n"; break;
        case 10: cout << "You are now proficient enough in sewing\nto make a mage hood.\n"; break;
        case 12: cout << "You are now proficient enough in sewing\nto make a quiver.\n"; break;
        case 14: cout << "You are now proficient enough in sewing\nto make a sheath.\n"; break;
    }

    //Add crafted item to inventory
    if(num == 7) pc.quiv = true;
    else if(num == 8) pc.sheath = true;
    else pickup( eq[ item ] );
}

void enchant()
{
	int item_i, ench_n, gem_i;
	string elem[7]={"null", "wind", "water", "earth", "fire", "lightning", "ice"};
	string gem[7]={"null", "crystal", "sapphire", "emerald", "ruby", "amethyst", "diamond"};
	int gem_ids[7] = {0, 46, 51, 50, 52, 53, 47};

	if(pc.mp < ( 2 * pc.enchlvl ) ) {prln("Not enough mana!"); return;}

	pc.showInv();
	cout << "Enchant which item? (MP cost: " << 2*pc.enchlvl << ")\n";
	item_i = check_int();

	//Check to see if item is already enchanted
	if(pc.inv[item_i].ench!="none") {cout<<"That item already has an enchantment.\n"; return;}

	//Make sure the selected item is the right type
	bool is_equipment = false;
	if ( item_i < 11 && (pc.inv[ item_i ].type == "weapon" || pc.inv[ item_i ].type == "armor" || pc.inv[ item_i ].type == "accessory") ) is_equipment = true;
	if ( !is_equipment ) {prln("That sort of thing won't hold any enchantment."); return;}

        cout<<"\nWhat sort of enchantment?\n";
        cout << "\n\n|__Enchantment__|___Gem________\n";
        for(int x = 1; x < 7; x++)
            {cout << "[" << x << "]" << " " << elem[x] << "  \t| " << gem[x] << "\t[Bag: " << pc.invCount("none", gem[x]) << "]\n";}
        cout << "[7] Return\n\n";
        do {ench_n = pc.getInpn();} while (ench_n > 8 && ench_n < 0);

        //Check player inventory for the correct gem
		gem_i = searchinv( gem_ids[ ench_n ] );
		if (!gem_i) {prln("You need " + aoran( eq[ gem_ids[ ench_n ] ].name , false) + eq[ gem_ids[ ench_n ] ].name + " to create that enchantment."); return;}

        //Set item enchantment
        pc.inv[ item_i ].ench = elem[ ench_n ];

        //Enchantment bonus based on enchanting level, max 5
        int enchb = rollfz( pc.enchlvl + 1 );
        pc.inv[ item_i ].price += (enchb * 10);
        if(enchb > 5) enchb = 5;
        if(enchb <= 0) enchb = 1;
        pc.inv[ item_i ].enchb += enchb;

        //Set item stat bonus based on enchantment level, max 5
        int eb = enchb;
        if( eb < 0 ) eb=0;
        if( eb > 5 ) eb=5;

        //Increase item stats by stat bonus
        switch(ench_n) {
            case 1: pc.inv[item_i].dxb+=eb; break;//Air, dexterity
            case 2: pc.inv[item_i].intb+=eb; break;//Water, intellect
            case 3: pc.inv[item_i].defb+=eb; break;//Earth, defense
            case 4: pc.inv[item_i].strb+=eb; break;//Fire, strength
            case 5: pc.inv[item_i].lckb+=eb; break;//Lightning, luck
            case 6: pc.inv[item_i].atkb+=eb; break;//Ice, attack
        }

    cout<<"\nYou use a "<<pc.inv[gem_i].name<<" to channel "<<elem[ench_n]<<" \nenergy into the item. The gem shatters,\nbut the item now glows faintly with mystic power.\n";

    //Remove gem
    pc.carrywt-=pc.inv[gem_i].wt;
    pc.inv[gem_i]=eq[0];
    //Mana cost
    pc.mp-=2*pc.enchlvl;
    //Enchanting XP
    if (pc.enchlvl < 20) pc.enchlvl++;

    //Trophy chance
    if(!trophy_list[trophyGetID("obelisk")].unlock&&rollfor(1,100)) trophyGet("obelisk", 0);
}

void cook()
{
	int num;
	bool ckchance = roll20( pc.cklvl + (pc.lck / 2) );
	bool oven;

	//if there's a building with an oven nearby
	if(pc.area.loc_bldg.name!="null")
        for(int ct=0; ct<pc.area.loc_bldg.objects.size(); ct++)
            if(pc.area.loc_bldg.objects[ct].name=="oven") {oven = true; break;}

    //if there's a player-built oven in the area itself
    for(int ct=0; ct<pc.area.obj.size(); ct++)
    {
        if(pc.area.obj[ct].name=="oven")
        {
            oven=true;
            break;
        }
    }
    //if player has a cookfire nearby or there's a static oven nearby
	if(pc.area.fire==true||pc.area.name=="Village"||pc.area.name=="City Center"||pc.area.name=="Winterhold"||oven)
	{
		prln("Cook item from which inventory slot?");
		num = pc.getInpn();

		if(pc.inv[num].mat=="raw")
		{
			if(ckchance)
			{
			    //If cooking a fish, add item weight to hitpoint gain
			    int hpg = 0; if(pc.inv[num].name == "fish") hpg = pc.inv[num].wt;

			    //Cooking an item with material type "raw" will transform it into the next item in the index
			    //("cooked" variants always follow "raw" directly)
                pc.inv[num]=eq[pc.inv[num].id+1];
                //Add extra hitpoint gain (zero by default)
                pc.inv[num].enchb += hpg;
                //Cooking XP gain
                pc.cklvl++;
			cout<<"You roast the raw food over your campfire.\n";
			if(!trophy_list[trophyGetID("roast")].unlock&&rollfor(1,100)) trophyGet("roast", 0);
			}
			else
			{
				cout<<"You accidentally burn the "<<pc.inv[num].name<<".\n";
                pc.inv[num]=eq[pc.inv[num].id-1];
			}
		}
		else if(pc.inv[num].name=="pot")
        {
            equipment tempit1, tempit2;
            int x, y, z, a;
            cout<<"You attempt to cook a stew...\n";

            if(!roll20(pc.cklvl + pc.lck)) {cout<<"Looks like it didn't get hot enough.\n"; return;}

            //search for food item one, return if none found
            x=searchinvtype("food"); if(x==0){cout<<"You don't have any food to cook!\n"; return;}
            tempit1=pc.inv[x];
            //search for food item two
            for(y=x+1; y<11;y++){if(pc.inv[y].type=="food"){tempit2=pc.inv[y]; break;}} if(y>10){cout<<"You need two food items to make a stew.\n"; return;}

            //search for iron pot to cook in
            z=searchinv(getItemID("iron", "pot")); if(z>10){cout<<"You need a pot to cook in.\n"; return;}
            //Check if pot is full of water; container must be pot
            if(pc.inv[z].ench!="water"){cout<<"You'll need water to make a stew.\n"; return;}

            //fill the pot, remove food items and water
            pc.inv[a].ench="none";
            pc.inv[z].ench=tempit1.name+"-"+tempit2.name+"-stew";

            //healing bonus is twice the sum of items used
            debugMsg("Item 1 enchb", tempit1.enchb);
            debugMsg("Item 2 enchb", tempit2.enchb);
            pc.inv[z].enchb = 2 * ( tempit1.enchb + tempit2.enchb );
            cout << "\nDebug: enchb set to: " << pc.inv[x].enchb << "\n";

            pc.inv[x]=eq[0]; pc.carrywt-=tempit1.wt;
            pc.inv[y]=eq[0]; pc.carrywt-=tempit2.wt;

            pc.cklvl++; pc.xp++;

            cout << "You make a pot of " << pc.inv[x].ench << " stew.\n";
            if(!trophy_list[trophyGetID("roast")].unlock&&rollfor(1,100)) trophyGet("roast", 0);
        }
	}
	else
		cout<<"There's no fire here.\n";
}

void bake()
{
    string bkables[6]={"Bread", "Redberry Pie", "Greenberry Pie", "Apple Pie", "Meat Pie", "Cake"};

    //Search inventory for ingredients first
    int ap=searchinv(getItemID("tsir", "apple"));
    int mt=searchinv(getItemID("raw", "meat"));
    int eg=searchinv(getItemID("none", "egg"));
    int gb=searchinv(getItemID("green", "berry"));
    int fl=searchinv(86);//flour
    int rb=searchinv(83);//redberry
    int cont=searchinvtype("container");
    int num=0;
    bool oven;

	//if there's a building with an oven nearby
	if(pc.area.loc_bldg.name!="null")
    {
        for(int ct=0; ct<pc.area.loc_bldg.objects.size(); ct++)
        {
            if(pc.area.loc_bldg.objects[ct].name=="oven")
            {
                oven=true;
                break;
            }
        }
    }

    //ovens available in -v
    if(pc.area.name=="Village"||pc.area.name=="City Center"||pc.area.name=="Winterhold")
    {
        cout<<"What do you want to bake?\n";
        for(int x=0; x<=pc.cklvl/3;x++)
            cout<<x+1<<": "<<bkables[x]<<"\n";

        num=check_int();

        switch(num)
        {
            case 1://bread - requires flour and water
            {
                if(fl>0&&pc.inv[cont].ench=="water")
                {
                cout<<"You mix the flour and water and bake a loaf of bread.\n";
                pickup(eq[getItemID("none", "bread")]);
                //Add cooking xp
                if (pc.cklvl < 20) pc.cklvl++; pc.xp;
                //Remove ingredients (flour, water)
                pc.inv[ fl ] = eq[0];
                pc.inv[ cont ].ench = "none";

                if(!trophy_list[trophyGetID("cake")].unlock&&rollfor(1,100)) trophyGet("cake", 0);
                }
                else cout<<"You don't have the right ingredients to bake that.\n";
            }
            break;

            case 2://redberry pie - flour, water, berry
            {
            if(fl>0&&rb>0&&pc.inv[cont].ench=="water")
            {
                cout<<"You mix the ingredients together and bake a redberry pie.\n";
                pickup(eq[87]);
                pc.inv[fl]=eq[0];
                pc.inv[rb]=eq[0];
                pc.inv[cont].ench="none";
                pc.cklvl++;
                if(!trophy_list[trophyGetID("cake")].unlock&&rollfor(1,100)) trophyGet("cake", 0);
            }
            else cout<<"You don't have the right ingredients to bake that.\n";
            }
            break;

            case 3://greenberry pie - flour, water, berry
            {
            if(fl>0&&gb>0&&pc.inv[cont].ench=="water")
            {
                cout<<"You mix the ingredients together and bake a greenberry pie.\n";
                pickup(eq[88]);
                pc.inv[fl]=eq[0];
                pc.inv[rb]=eq[0];
                pc.inv[cont].ench="none";
                if (pc.cklvl < 20) pc.cklvl++;
                if(!trophy_list[trophyGetID("cake")].unlock&&rollfor(1,100)) trophyGet("cake", 0);
            }
            else cout<<"You don't have the right ingredients to bake that.\n";
            }
            break;

            case 4://apple pie
            {
            if(fl>0&&ap>0&&pc.inv[cont].ench=="water")
            {
                cout<<"You mix the ingredients together and bake an apple pie.\n";
                pickup(eq[getItemID("apple", "pie")]);
                pc.inv[fl]=eq[0];
                pc.inv[rb]=eq[0];
                pc.inv[cont].ench="none";
                if (pc.cklvl < 20) pc.cklvl++;
                if(!trophy_list[trophyGetID("cake")].unlock&&rollfor(1,100)) trophyGet("cake", 0);
            }
            else cout<<"You don't have the right ingredients to bake that.\n";
            }
            break;

            case 5://meat pie
            {
            if(fl>0&&mt>0&&pc.inv[cont].ench=="water")
            {
                cout<<"You mix the ingredients together and bake a meat pie.\n";
                int mp=getItemID("meat", "pie");
                cout<<"\n"<<mp<<"\n";
                pickup(eq[mp]);
                pc.inv[fl]=eq[0];
                pc.inv[mt]=eq[0];
                pc.inv[cont].ench="none";
                if (pc.cklvl < 20) pc.cklvl++;
                if(!trophy_list[trophyGetID("cake")].unlock&&rollfor(1,100)) trophyGet("cake", 0);
            }
            else cout<<"You don't have the right ingredients to bake that.\n";
            }
            break;

            case 6://cake
            {
            if(fl>0&&eg>0&&pc.inv[cont].ench=="water")
            {
                cout<<"You mix the ingredients together and bake an apple pie.\n";
                pickup(eq[getItemID("none", "cake")]);
                pc.inv[fl]=eq[0];
                pc.inv[eg]=eq[0];
                pc.inv[cont].ench="none";
                if (pc.cklvl < 20) pc.cklvl++;
                if(!trophy_list[trophyGetID("cake")].unlock&&rollfor(1,100)) trophyGet("cake", 0);
            }
            else cout<<"You don't have the right ingredients to bake that.\n";
            }
            break;

            default:{cout<<"You don't have the right ingredients to bake that.\n";} break;
        }
    }
    else
    {
        cout<<"You can't find an oven nearby.\n";
    }
}


void questlog()
{
    if(game_settings.sound) mciSendString(sdPaper.c_str(), NULL, 0, NULL);
	cout<<"\nQuests:\n\n";
	for(int x=0;x<25;x++)
	{
		if(qlog[x].lvl>0){
			cout<<qlog[x].id<<". "<<qlog[x].name<<"\n";
            if(qlog[x].comp!=true)
                cout<<qlog[x].desc[qlog[x].lvl]<< "\n";
            else
                cout<<"-[Complete!]\n\n";}
	}

    switch(pc.fairy_missive)
    {
        case 1: cout<<"You've found the Fay Court - and the Queen wants you to deliver a message to her daughter.\n"; break;
        case 2: cout<<"The Fay Princess has sent you back to the Court with a token of her response.\n"; break;
        case 3: cout<<"You've settled things between the Fay Queen and her daughter, earning their favor.\n"; break;
        default: break;
    }
	if(m_data.lost_in_party) cout<<"\n-There is "<<aoran(lost.age, false)<<lost.showName("agemf")<<" traveling with you.\nYou're taking them to "<<m_data.lostx<<", "<<m_data.losty<<", '"<<loc[m_data.lostx][m_data.losty].name<<"'.\n";
	cout<<"\n";
}

void inv()
{
    if(game_settings.sound) mciSendString(sdBag.c_str(), NULL, 0, NULL);

	cout<<"Weapon: "<<pc.weap.showName()<<"\n";
	cout<<"Armor: "<<pc.arm.showName()<<"\n";
	cout<<"Accessory: "<<pc.acc.showName()<<"\n";

	if(pc.sheath==true) cout<<"Sheath: "<<pc.sheathed.showName()<<"\n";
	if(pc.quiv==true) cout<<"Quiver ";
	if(pc.arrows)cout<<"(Arrows: "<<pc.arrows<<")\n";

	//Quest / Unique Items
	switch(pc.fairy_missive)
	{
        case 1: cout<<"- A tightly ribboned missive bearing the mark of the Fae\n"; break;
        case 2: cout<<"- A sparkling brooch from a Fae princess\n"; break;
        case 3: cout<<"- A glowing moonstone ring bearing the mark of the Fae\n"; break;
	}

	cout<<"\nPotions:\n";
	cout<<"Health: "<<pc.hpot<<"   Mana: "<<pc.mpot<<"   Antidote: "<<pc.apot<<"\n";
	cout<<"Gold: "<<pc.gp<<"\n\n";
	cout<<"Inventory:\n";
	for(int x=1;x<11;x++) cout<<"("<<x<<"): " << pc.inv[x].showName()<<"\n";
	cout<<"\n";
}

void show_tools()
{
    if(game_settings.sound) mciSendString(sdBag.c_str(), NULL, 0, NULL);
	cout<<"\nTools:\n\n";
	if(pc.frod>0) cout<<"-Fishing Rod";
	if(pc.frod==2) cout<<" & lure";
	if(pc.frod>0)cout<<" (Bait: "<<pc.bait<<")\n";
	if(pc.ham==true) cout<<"-Hammer\n";
	if(pc.chis==true) cout<<"-Chisel\n";
	if(pc.pick==true) cout<<"-Pickaxe\n";
	if(pc.axe==true) cout<<"-Woodcutting axe\n";
	if(pc.tbox==true) cout<<"-Tinderbox\n";
	if(pc.need==true) cout<<"-Needle & Thread\n";
	if(pc.shears==true) cout<<"-Shears\n";
}

void showSpells()
{
    if(game_settings.sound) mciSendString(sdPaper.c_str(), NULL, 0, NULL);
    int spells_total = 0;
    prln("Spells learned: \n");

    cout << "ID  | Name      \t| Element  \t| Damage       \t | Mana Cost \n";
    cout << "----+-------------------+---------------+----------------+-------\n";

	for(int x=0; x<68;x++)
	{
		if(splist[x].unlock)
		{
		    spells_total++;
            cout << " " << x << " | " << splist[x].name << "      \t| " << splist[x].elem << "    \t| " << splist[x].dmg << "-" << splist[x].maxd << "     \t| " << splist[x].cost;
            cout << "\n";
		}
	}
	if(spells_total <= 0) prln("You don't know any spells or abilities yet!");
}

void play()
{
	int sid;
	string notes_tot="null";

	int x=searchinvtype("instrument");
    if(x>0){
            char notes[5];
            int noten[5];
            cout << "Enter 4 notes (a, b, c, or d):" << endl;

            for(int x=1;x<5;x++)
            {
                cin>>notes[x];
                noten[x]=(abs(notes[x]-100)+1);
                notes[x]=toupper(notes[x]);
                if (x==1) notes_tot=notes[x];
                else notes_tot=notes_tot+notes[x];
            }

            for(int x=1;x<5;x++)
            {
                for(int y=1;y<5;y++)
                {
                    if(noten[y]==x) cout<<" -"<<(notes[y])<<"-- ";
                    else cout<<" ---- ";
                }
                cout<<"\n";
            }

	for(int y=0;y<10;y++)
	{
		if(songs[y].score==notes_tot)
		{
			sid=songs[y].id;
			//If song is played for the first time
			if(!songs[y].learned)
            {
                songs[y].learned = true;
                prln("You've learned a new song!");
            }
			cout<<"\nYou play "<<songs[y].name<<".\n";
			cout<<songs[y].desc<<"\n";
			pc.muslvl++;
            if(!trophy_list[trophyGetID("flute")].unlock&&rollfor(1,100)) trophyGet("flute", 0);
			break;
		}
	}
    if(roll20(pc.muslvl)){
    switch(sid)
    {
        case 0://battle march
	{
		pc.area.echance*=2;
		cout<<"DEBUG: echance: "<<pc.area.echance<<"\n";
	}
        break;
        case 1://seasons' hymn
	{
		curs=seasons[curs.id+1];
		cout<<"DEBUG: Season: "<<curs.name<<"\n";
	}
        break;
        case 2://sunrise melody
	{
		timen=0;
		cout<<"DEBUG: time of day: "<<timen<<"\n";
	}
        break;
        case 3://serenity
	{
		pc.area.echance=0;
		cout<<"DEBUG: echance: "<<pc.area.echance<<"\n";
	}
        break;
        case 4://snowfall
	{
		curs.weath=cweath[1];
		cout<<"DEBUG: weather: "<<curs.weath.name<<"\n";
	}
        break;
        case 5://song of storms
	{
		curs.weath=cweath[5];
		cout<<"DEBUG: weather: "<<curs.weath.name<<"\n";
	}
        break;
        case 6://lucky cat
	{
			chestroll();
			if(pc.area.chest <=0)
            {
                int gpg = roll(pc.lck + pc.karma);
                pc.gp += gpg;
                prln("You found some coins in the dirt.");
            }
	}
        break;
        case 7://spring song
	{

		loc[1][6].crops=true;
		loc[2][7].crops=true;
		if(loc[1][6].crops=true)
        {
            cout<<"DEBUG: crops west: true\n";
        }
        if(loc[2][7].crops=true)
        {
            cout<<"DEBUG: crops west: true\n";
        }
		pc.area.load_flora();
		cout<<"DEBUG: display environment: ";pc.area.display_env();
	}
        break;
        case 8://rogue's lament
	{
		cout<<"You find some coins on the ground! What luck!\n";
		pc.gp+=roll(pc.muslvl+pc.lck);
	}
        break;
        case 9://starlight aria
	{
		timen=8;
	}
        break;
    }}
    else cout<<"Nothing happens...maybe practice a little more?\n";
    }//if player has instrument
    else cout<<"You try to whistle a song, but you'd need an instrument to do it justice.\n";
}

void fairy_village()
{
    colSet(250, "   Fairy Village\n");
    cout<<"\nYou stumble over something...a toadstool?\nSuddenly, as if they'd been there all along, you see\ndozens of delicate fairies, busily fluttering between toadstool homes\nand treetop courts. A few flutter up to you, offering a silent but cheerful greeting.\n";
    do{
    cout<<"(1) Talk\n(2) Shop\n(3) Search\n(4) Seek an audience\n(5) Leave the village\n";
    cin>>pc.inp;

    if(pc.inp=="1")
    {
        int x=rand()%7;
        int y=rand()%16;
        int z=rand()%10;
        string color[16]={"red", "blue", "yellow", "green", "purple", "orange", "teal", "cerulean", "silver", "gold", "pink", "white", "black", "violet", "scarlet", "rose"};
        string garb[10]={"tunic", "hat", "scarf", "dress", "skirt", "cloak", "hood", "sash", "gown", "robe"};
        string fairy_chatter[7]={
            "'You should try living in a toadstool sometime! It's sublime!'\n",
            "'Fairy magic is powerful, but we can't manipulate the elements like the other races.'\n",
            "'Steer clear of those nasty pixies, they're nothing but trouble!'\n",
            "'The princess has been gone for awhile now...\nI'm sure she's out there somewhere.'\n",
            "'Have you been to see the Queen?\n...she only likes good people, you know!'\n",
            "'Strange things sometimes find their way here...'\n",
            "'You know we can't live in bottles, right?\n...No reason.'\n"};

        cout<<"A fairy in a "<<color[y]<<" "<<garb[z]<<" flits past, returning a moment to say,\n";
        cout<<fairy_chatter[x];
        cout<<" before flying on.\n";
    }
    else if(pc.inp=="2")
    {
        int flora_wares[10] = {73, 83, 84, 117, 118, 119, 120, 121, 103, 104};
        int magic_wares[10] = {46, 47, 50, 51, 52, 53, 67, 68, 92, 93};

        int w1 = flora_wares[rand()%10];
        int w2 = flora_wares[rand()%10];
        int w3 = magic_wares[rand()%10];
        int w4 = magic_wares[rand()%10];
        int wares[4] = {w1, w2, w3, w4};

        shops[12] = {
            "Fairy Shop",
            "A bored-looking fairy is slouched against an acorn-shell\ncounter, filing her nails. She blows a strand of purple hair out of her eye.\n",
            "'You gonna buy something?'\n",
            "She ignores you. ",
            "'Excellent choice.'\n",
            "'Sure, whatever. Bye.'\n",
            5, 12,
            "A quaint, tiny shop"
        };

        for(int x=0;x<4;x++) {shops[12].wares_id.push_back(wares[x]);}

        create_shop(12, "Fairy Shop");
    }
    else if(pc.inp=="3")
    {
        prln("You search the area for anything useful.");
        if(rollfor(pc.lck+pc.karma, 100))
        {
            cout<<"You found an item!\n";
            pickup(eq[rand()%300]);
        }
        else cout<<"You don't find anything interesting.\n";
    }
    else if(pc.inp=="4")
    {
        if(pc.fairy_missive==1) cout<<"\n'Please, find my daughter.'\n";
        else if(pc.fairy_missive==2)
        {
            cout<<"\nThe Queen tenderly accepts the brooch, tears welling in her dark eyes.\n'Thank you, traveler. At the very least I know she is alive and well;\nlet her find her fortune where she may.'\n";
            cout<<"She turns to you and smiles. 'Please, accept this enchanted ring as a token of my gratitude.\nI would also like to bestow upon you a gift from the Court...'\n";
            do{
                cout<<"\n(1) Moonbeam blade\n(2) Fairy dust\n(3) Gossamer wings\n";
                pc.inp = pc.getInps();
                if(pc.inp=="1") { pickup(eq[getItemID("moonbeam", "blade")]); }
                else if(pc.inp=="2") { pickup(eq[getItemID("fairy", "dust")]); }
                else if(pc.inp=="3") { pickup(eq[getItemID("gossamer", "wings")]); }
            } while(pc.inp!="1"&&pc.inp!="2"&&pc.inp!="3");

            pc.fairy_missive = 3;
            cout<<"You reverently accept the item, and the Queen smiles.\n'Thank you for your help, mortal.'\n";
            cout << "\nYou inspect the Queen's ring, a beautiful moonstone in a shining copper band.\nIt appears you can return to the Fae Courts with the command 'ring'.\n";
        }
        else if(pc.fairy_missive==3)
        {
            cout<<"The Queen smiles gently. 'Thank you for your help, mortal.'\n";
            if(!trophy_list[trophyGetID("waterfall")].unlock&&rollfor(1,100)) trophyGet("waterfall", 0);
        }
        else if(pc.karma>=20&&pc.fairy_missive<1)
        {
            cout<<"\nYou enter into the court of the Faerie Queen, a sparkling glade full of\nofficious looking faeries seated around an overgrown throne.\nThe queen herself is much larger than the other Fae, nearly as tall as a dwarf.\nShe radiates an aura of stately, alien beauty, commanding the attention of all present.\n";
            cout<<"\n'Traveler,' she says, 'I know not how you came into this sacred glade.\nHowever, it is fortunate that you have; I have need of one that can\ntraverse the world outside.\n\nShe looks at you seriously. 'My daughter, our Princess, has left my court; I worry for her safety.\n\n";
            cout<<"The Queen conjures a tightly rolled scroll, passing it to you.\n'Please, if you see her, give her this missive.\n";
            pc.fairy_missive=1;
        }
        else cout<<"The Faerie Queen declines the audience, urging you to listen more closely\nto the needs of the land.\n";
    }
    else if(pc.inp=="5")
    {
        cout<<"Looking over your shoulder wistfully, you leave the village, holding tight to the magical memory.\n";
        if(!trophy_list[trophyGetID("toadstool")].unlock&&rollfor(1,100)) trophyGet("toadstool", 0);
    }
    else cout<<"Sorry, I don't understand...please choose one of these options: \n";
    } while(pc.inp!="5");
}

void wizardTower()
{
    bool bird = false;
    bool balloon = false;
    bool balls = false;
    bool is_open = false;

    string inp;


    colSet(COL_BKONWH, "Wizard's Tower");
    cout << "\n\n\n";
    //Description of entry, tower interior
    cout << "Emerging at last from the crevices and precipices of the\nnarrow mountain path, you come upon a stone door at the base of a steep rise.\n";
    cout << "After an endless flight of thinly carpeted stone stairs, you enter into in a brightly lit laboratory.\n\n";

    cout << "Oaken bookshelves line the smooth stone walls, and\ntables cluttered with equipment and charts are\ntastefully arranged around the room.\n\n";

    //Characters
    colSet(COL_BLUE, "Frost"); cout << " is studying a crystal orb nearby, taking notes with a giant quill.\n";
    cout << "A familiar-looking tiny "; colSet(COL_YELLOW, "Wizard"); cout << " is staring out the window at the snow.\n";
    //Nearby objects
    cout << "A dipper "; colSet(COL_CYAN, "bird");
        if ( !bird ) cout << " is attempting to bob at a glass of water.\n";
        else cout << " is bobbing happily into a glass of water.\n";
    cout << "A small "; colSet(COL_YELLOW, "balloon");
        if ( !balloon ) cout << " hangs limp over an unlit candle.\n";
        else cout << " is hovering over a flickering candle.\n";
    cout << "A series of silver "; colSet(COL_PURPLE, "balls");
        if ( !balls ) cout << " hang neatly in a row on a nearby desk.\n";
        else cout << " clack back and forth in a pleasing rhythm.\n";
    if (is_open) { cout << "A glowing "; colSet(COL_CYAN, "portal"); cout << " hovers in the center of the room.\nThe two wizards pay it no mind.\n"; }


    //Begin main loop:
    do {

    //Get input
    cout << "\nWhat will you do?\n";
    inp = pc.getInps();
    cout << "~+--------------------+~";
    cout << "\n";

    //Parse input

    if ( inp == "bird" ) {
        if (bird) { bird = false; prln("You reach out and stop the bird's bobbing.\nIt seems nonplussed.");
            if (is_open) { is_open = false; prln("The glowing portal winnows to a close."); }
        }
        else {
            bird = true;
            prln("You reach out and give the bird a little push.\nIt begins to swing with its full range of motion, dipping its beak in the water.");
            if ( balls && balloon ) is_open = true;
        }
    }
    else if ( inp == "balloon" ) {
        if (balloon) { balloon = false; prln("You blow out the candle. The colorful balloon sinks forlornly.");
            if (is_open) { is_open = false; prln("The glowing portal winnows to a close."); }
        }
        else if ( balls ) { balloon = true; prln("You light the candle, holding the balloon over the\nyellow flame until it begins to float."); }
        else prln ("Nothing happens.");
    }
    else if ( inp == "balls" ) {
        if (balls) { balls = false; prln("You hold the balls still, halting their movement.");
            if (is_open) { is_open = false; prln("The glowing portal winnows to a close."); }
        }
        else { balls = true; prln("You pull back one of the balls, letting it swing into the next, and so on."); }
    }

    else if ( inp == "portal" && is_open ) {

        //X and Y coordinates for each teleport target
        // Red, green, blue

        //Goblin village, Ancient Forest Glade, Glacial Palace, Volcano, Throne Room, Village, Elven Village
        point_2d areas[7];
        areas[0] = {2, 2};
        areas[1] = {10, 6};
        areas[2] = {5, 1};
        areas[3] = {10, 1};
        areas[4] = {10, 4};
        areas[5] = {1, 10};
        areas[6] = {9, 9};

        point_2d targ = areas[ rollfz(7) ];

        pc.area = loc[ targ.x ][ targ.y ];

        cout << "\nYou walk through the portal.\nIt offers some resistance, like a gelatinous membrane.\n";
        prln("Your foot touches thin carpet on the other side...You're still in the tower laboratory.\nThe light coming in the tower's high windows has changed subtly.");
    }

    else if ( strCase(inp, "upper") == "Frost" ) {
        prln("The old wizard looks up from his orb.\n'Oh, hello. Have a look around if you want.'");
    }

    else if ( strCase(inp, "upper") == "Wizard" ) {
        prln("The tiny wizard looks over at you.\n'Oh! ...it's you.'");
    }

    else if ( inp == "area" || inp == "look" ) {
        colSet(COL_BKONWH, "Wizard's Tower");
        cout << "\n\n\n";

        //Interior description
        cout << "Oaken bookshelves line the smooth stone walls, and\ntables cluttered with equipment and charts are\ntastefully arranged around the room.\n\n";

        //Characters
        colSet(COL_BLUE, "Frost"); cout << " is studying a crystal orb nearby, taking notes with a giant quill.\n";
        cout << "A familiar-looking tiny "; colSet(COL_YELLOW, "Wizard"); cout << " is staring out the window at the snow.\n";
        //Nearby objects
        cout << "A dipper "; colSet(COL_CYAN, "bird");
            if ( !bird ) cout << " is attempting to bob at a glass of water.\n";
            else cout << " is bobbing happily into a glass of water.\n";
        cout << "A small "; colSet(COL_YELLOW, "balloon");
            if ( !balloon ) cout << " hangs limp over an unlit candle.\n";
            else cout << " is hovering over a flickering candle.\n";
        cout << "A series of silver "; colSet(COL_PURPLE, "balls");
            if ( !balls ) cout << " hang neatly in a row on a nearby desk.\n";
            else cout << " clack back and forth in a pleasing rhythm.\n";
        if (is_open) { cout << "A glowing "; colSet(COL_CYAN, "portal"); cout << " hovers in the center of the room.\nThe two wizards pay it no mind.\n"; }

    }
    //Exit case
    else if ( inp == "exit" || inp == "leave" ) prln("You descend the staircase and exit the tower.");

    //Error case
    else prln("I'm sorry, I don't understand...\nPlease try again or type 'exit' to leave.");

    //Formatting
    cout << "\n";
    cout << "~+--------------------+~";
    cout << "\n";

    //End main loop on exit commands
    } while ( inp != "back" && inp != "leave" && inp != "exit" );

    cout << "\n";
    colSet(COL_BKONWH, pc.area.name);
    cout << "\n\n";
    prln(pc.area.desc);
    pc.area.display_env();
}

void mountainPath()
{
    //Leads eventually from 7, 1 "Frost's Eyrie" to wizardTower()

    string path_desc[4] = {
        "You get lost in a maze of pitch-black caves.",
        "A narrow ledge gives way under you, sending you\nsliding back down in a tumble of loose scree.",
        "After climbing for hours, you find yourself at an infuriating dead end.\nYou retrace your steps.",
        "Your sore hands and feet at last bring you up the rock face to the path's end."};
    string inp = "null";
    int num = 0;

    do{
        prln("You attempt to navigate the treacherous path...");

        num = rollfz(4);

        prln( path_desc[ num ] );

        if (num < 3) {prln("Continue on? (Y / N)"); inp = pc.getInps();}

    } while ( num < 3 && !inpDeny(inp) );

    if ( inpDeny(inp) ) return;
    else wizardTower();
}

void nightMaze()
{
    bool has_light = false;
    string light_source = "none";

    if ( searchinv( getItemID("foul", "torch") ) ) { has_light = true; light_source = "torch"; }
    else if ( searchinv( getItemID("sacred", "torch") ) ) { has_light = true; light_source = "torch"; }
    else if ( ( searchinv( getItemID("glass", "bottle") ) ) &&
            ( pc.inv[ ( searchinv( getItemID("glass", "bottle") ) ) ].ench == "Firefly" ) ) { has_light = true; light_source = "firefly"; }


}

void hunt()
{
    //companion bonus
    int comp_bonus=0;
    if (pc.comp.name=="Luz") comp_bonus=pc.comp.lvl;//Luz adds her level to hunting chance

    //chance to find an animal
	bool animal_is_present=rollfor(pc.lck+pc.hlvl+comp_bonus, 20);//luck + hunting level + companion level out of 20
	if(pc.area.loc_fauna.name!="null") animal_is_present=true;

	//define the animal
        envinpc animal;
        //if there's an animal already present use that
        if(pc.area.loc_fauna.name!="null") {animal=pc.area.loc_fauna;}
        //otherwise grab one from the area's fauna list
        else{
            animal=pc.area.fauna_types[rand()%pc.area.fauna_types.size()];
            //set area fauna to local fauna
            pc.area.loc_fauna=animal;}
        //set level, change health and speed to match
        animal.lvl=rand()%11-5;
        animal.dex+=animal.lvl;
        animal.hp+=animal.lvl;

        if (animal.dex < 1) animal.dex = 1;
        if (animal.hp < 1) animal.hp = 1;

	if(animal_is_present)
	{
		cout<<"You spot a wild "<<animal.name<<" "<<animal.idle<<"! Hunt the animal? (y/n)\n";
		cin>>pc.inp;
		if(pc.inp=="y")
		{
            //determine success
            if(pc.weap.subt=="bow") {animal.dex-=pc.weap.atkb; cout<<"Your bow gives you an advantage.\n";}
            if(pc.comp.name=="Luz") {animal.dex-=2; animal.hp-=2; cout<<"Luz, hunting alongside you, looses an arrow into the beast's leg.\n";}
            int dex_chk=pc.dex-animal.dex;
            int str_chk=pc.str-animal.hp;
            bool success=0;
            if(dex_chk>=0&&str_chk>=0) success=1;

		if(success)
		{
                if (animal.name=="Fairy")
                {
                    cout<<"\nYou attempt to catch the wily Fae; slipping between your clumsy swipes like mist and moonbeams,\nthe fairy disappears between two moss-covered trunks.\nYou consider squeezing through after...\n";
                    cout<<"Enter Fairy Village?\n(1) Yes\n(2) No\n";
                    cin>>pc.inp;
                    if(pc.inp=="1")
                    {
                        m_data.fairy_loc=pc.area.name;
                        fairy_village();
                    }
                    return;
                }
                if (animal.name == "Frillneck") { elist[44].kills++; }

				cout<<"You carefully track the beast and kill it.\n";
				if(animal.drop != 0)
				{

					if(animal.desc == "bird")
					{
						cout<<"You pluck a feather from the corpse.\n";
						pickup( eq [ getItemID("none", "feather") ] );
					}
					if(animal.desc=="mammal"||animal.desc=="med mammal"||animal.desc=="large mammal")
					{
                        cout<<"You relieve the animal of its hide.\n";
                        for(int x = 1; x <= animal.drop; x++) pickup( eq[35] );
					}
				}
				if(animal.desc!="insect")
                {
                    cout<<"You butcher the "<<animal.name<<" for a chunk of raw meat.\n";

					if(animal.desc=="bird") pickup( eq[ getItemID("raw", "fowl") ] );
					else { pickup( eq[42] ); }//Raw meat
                }
					int xpg=rand()%abs(animal.lvl)+1;
					cout<<"Gained "<<xpg<<" xp for the successful kill.\n";
					pc.xp+=xpg;
					pc.area.loc_fauna=anim[0];
			}
			else{
                if(dex_chk<0) cout<<"The beast is too fast.\n";
                if(str_chk<0) cout<<"The beast is too strong.\n";
                cout<<"It evades your clumsy attacks.\n";
			}
			pc.hlvl++;
		}
		else
			"The creature slips away and is soon out of sight.\n";
	}
	else
		cout<<"You search the area for tracks but find nothing.\n";
}

void catch_npc()
{
    bool cch=rollfor(((pc.hlvl+pc.lck)-pc.area.loc_fauna.dex+1), 100);
    int rope=searchinv(getItemID("none", "rope"));
    int bottle=searchinv(getItemID("glass", "bottle"));
    int net=searchinv(getItemID("none", "net"));

    if(pc.area.loc_fauna.name!="null")
    {
        if((pc.area.loc_fauna.desc=="large mammal"||pc.area.loc_fauna.desc=="mammal")&&rope>0)
        {
            cout<<"You catch the "<<pc.area.loc_fauna.name<<" with a rope lead!\n";
            pc.inv[rope].ench=pc.area.loc_fauna.name;
            pc.inv[rope].subt="pet";
            pc.inv[rope].enchb=pc.area.loc_fauna.id;
            pc.area.loc_fauna=anim[0];
        }
        else if(pc.area.loc_fauna.desc=="small mammal"&&net>0)
        {
            cout<<"You catch the "<<pc.area.loc_fauna.name<<" in a net!\n";
            pc.inv[net].ench=pc.area.loc_fauna.name;
            pc.inv[net].subt="pet";
            pc.inv[net].enchb=pc.area.loc_fauna.id;
            pc.area.loc_fauna=anim[0];
        }
        else if((pc.area.loc_fauna.desc=="insect"||pc.area.loc_fauna.desc=="amphibian")&&bottle>0)
        {
            cout<<"You catch the "<<pc.area.loc_fauna.name<<" in a glass bottle!\n";
            pc.inv[bottle].ench=pc.area.loc_fauna.name;
            pc.inv[bottle].subt="pet";
            pc.inv[bottle].enchb=pc.area.loc_fauna.id;
            pc.area.loc_fauna=anim[0];
                if(pc.area.loc_fauna.name=="firefly"&&!trophy_list[trophyGetID("firefly")].unlock&&rollfor(1,100)) trophyGet("firefly", 0);
        }
        else
            cout<<"You don't have the right tools to catch that animal!\n";
    }
    else
    {
        cout<<"There aren't any animals nearby you can catch.\n";
    }
}

void release()
{
    int pet=searchinvsubt("pet");
    int inp;

    if(pet==0||pet>11)
    {
        cout<<"You don't have any pets to release!\n";
        return;
    }

    cout<<"Really release the "<<pc.inv[pet].ench<<"?\n(1) Yes\n(2) No\n";
    inp=check_int();

    if(inp==1)
    {
        cout<<"You release the "<<pc.inv[pet].name<<" into the wild.\nIt is now "<<anim[pc.inv[pet].enchb].idle<<" nearby.\n";
        pc.inv[pet].ench="none";
        pc.inv[pet].subt="none";
        pc.area.loc_fauna=anim[pc.inv[pet].enchb];
    }
}

void theLost(int state)
{
    string age, adj, n, pos, pers;
    switch(state)
    {
        case 0:
            {
                do{
                    m_data.lostx=rand()%10+1;
                    m_data.losty=rand()%10+1;
                } while (loc[m_data.lostx][m_data.losty].name=="River");
                lost.initialize(0);
                age=lost.age;
                adj=lost.adj;
                n=lost.n;
                pos=lost.pos;
                pers=lost.pers;

              cout<<"\n"<<aoran(adj, 1)<<lost.showName("full")<<" in simple garb\nhails you from the side of the path.\n\n'Traveler! I could use some assistance.'\n"<<lost.pers_up<<" leans heavily on "<<lost.pos<<" traveling staff with a wry look.\n";
              cout<<"\n'I'm lost, you see. If you could take me with you as far as\nthe "<<loc[m_data.lostx][m_data.losty].name<<", I'd be incredibly grateful.'\n";
              cout<<"(1) Yes\n(2) No\n";
              pc.inp = pc.getInps();
              if( inpAffirm( pc.inp ) )
              {
                  cout<<"The "<<n<<" claps "<<pos<<" hands together. 'It's settled then! Lead on, bold adventurer!'\n";
                  m_data.lost_in_party=1;
                  return;
              }
              else
              {
                  cout<<"\n"<<lost.pers_up<<" nods understandingly. 'Yes, of course. Better things to do than\ntote this old body around. Well, no time to waste!'\n\nSoon the "<<lost.showName("")<<" is lost in the distance.\n";
                  return;
              }
            }
        break;
        case 1:
            {
                cout<<"Your travelling companion takes a deep breath, looking around. 'Well, this is it, huh?\n\nThanks for all your help.\n\nThis might be of some worth to you.'\n";
                cout<<lost.pers<<" rummages for a trinket from "<<lost.pos<<" pack, and, setting it between you, departs with a cheery wave.\n";
                pc.area.loot=itemroll(30, 1);
                int xpg=rand()%pc.intl+1;
                int gpg=rand()%pc.lck+1;
                cout<<"\nMini-quest Complete!!\nGained "<<xpg<<"XP, and "<<gpg<<" Gold.\n";
                if(game_settings.sound) mciSendString(sdGlimmer.c_str(), NULL, 0, NULL);
                pc.xp+=xpg;
                pc.gp+=gpg + pc.karma;
                m_data.lostx=999; m_data.losty=999;
                m_data.lost_in_party=false;
                setKarma(pc.karma+roll(5), false);
                if(!trophy_list[trophyGetID("signpost")].unlock&&rollfor(pc.karma,1000)) trophyGet("signpost", 0);
            }
        break;
    }
}

void theWeak()
{
    if(!pc.area.foe.alive) {pc.area.foe=elist[pc.area.enemies[2]]; pc.area.foe.alive = true;}
    m_data.weak_nearby=true;
    weak.initialize(0);

    cout<<"\nYou hear panicked screaming as a nearby "<<weak.showName("n")<<" is menaced by a wild "<<pc.area.foe.name<<"..!\n";
    cout<<"Help them?\n";
    cout<<"(y/n)\n";
    cin>>pc.inp;
    if(pc.inp=="y"||pc.inp=="yes")
    {
        combat();
    }
    else {cout<<"\nYou shield your eyes as the poor "<<weak.showName("n")<<" is torn to shreds...\n"; m_data.weak_nearby=false; pc.karma-=2;}
}



void enchantedWell()
{
    prln("You spot an old, crumbling stone well hidden nearby.");
    prln("There's still a bucket attached, hanging from a functioning pulley.");
    cout<<"\nDrink from the well? (Y / N)\n";
    cin>>pc.inp;

    if(inpAffirm(pc.inp))
    {
        string w_types[3] = {"clear", "sparkling", "brackish"};
        int w_type = rollfz(3);
        string water = w_types[w_type];

        prln("You drop the bucket into the well and pull it up.");
        prln("It is full of "+water+" water.\nYou drink deeply.");

        switch(w_type)
        {
            case 0: //Clear water
            {
                int gain = rand()%10+5;
                int stat = roll(2);
                string s_stat[3] = {"", "health", "mana"};

                switch(stat)
                {
                    case 1: pc.hp += gain; break;
                    case 2: pc.mp += gain; break;
                    default: cout<<"Nothing happens.\n";
                }
                cout<<"\nThe water refreshes you.\nGained "<<gain<<" "<<s_stat[stat]<<".\n";
            }
            break;
            case 1: //Sparkling water
            {
                int gain = rand()%17+13;
                int stat = roll(2);
                string s_stat[3] = {"", "health", "mana"};

                switch(stat)
                {
                    case 1: pc.hp += gain; break;
                    case 2: pc.mp += gain; break;
                    default: cout<<"Nothing happens.\n";
                }

                cout<<"\nYou feel invigorated!\nGained "<<gain<<" "<<s_stat[stat]<<".\n";
            }
            break;
            case 2: //Brackish water
            {
                int loss = roll(20);
                int stat = roll(2);

                switch(stat)
                {
                    case 1: pc.hp -= loss; break;
                    case 2: pc.mp -= loss; break;
                    default: cout<<"Nothing happens.\n";
                }

                prln("It tastes like something died in it...\nYou spit it out and retch miserably.");
            }
            break;
            default: cout<<"Nothing happens.\n"; break;
        }
    }//if(inp=y)
    else
    {
        prln("You decide against drinking from the strange well.");
        return;
    }
}

void Gatherer()
{
    int n_flid[11] = {62, 83, 84, 103, 104, 105, 117, 118, 119, 120, 121};
    int r_flid = rollfz(11);
    equipment flora = eq[n_flid[r_flid]];
    prln("You see a woman gathering herbs among the trees nearby.\n\nShe smiles as she sees you.");
    prln("'Here,' she says, 'You can have some of these.'");
    prln("She hands you a "+flora.showName()+"!");
    pickup(flora);
}

void Shadow(int level)
{
    if(timen != 10)
    {
        prln("A sudden chill makes you shiver.\nYou look around, but there's nothing there.");
        return;
    }
    else
    {
        prln("A sudden chill makes you shiver.");
        prln("You whirl around, heart pounding, to see a shadow standing in the moonlight.");
        prln("It's "+aoran(pc.race, false)+pc.race+" "+pc.clas+",\nall shadowed form and gleaming eyes,\nholding "+aoran(pc.weap.mat, false)+pc.weap.showName()+" loosely at its side...");

        //name, hp, mp, atk, def, lvl, s/d/i/l, xp, gp, alive, idling text, element, weakness,
        enem dark_player = {
            "Dark "+pc.name,
            "warrior",
            pc.hpmax,
            pc.mpmax,
            pc.atk-pc.weap.atkb,
            pc.def-(pc.weap.defb+pc.arm.defb+pc.acc.defb),
            pc.lvl,
            pc.str,
            pc.dex,
            pc.intl,
            pc.lck,
            (pc.xp*pc.lvl)/2,
            pc.gp,
            false,
            "dancing in the moonlight",
            "dark",
            "none"};
        dark_player.ab = splist[0];
        while (!dark_player.ab.unlock){
            dark_player.ab = splist[rollfz(68)];
        }
        if(pc.weap.subt == "bow") dark_player.type = "archer";

        pc.area.foe = dark_player; pc.area.foe.alive = true;

        combat();
    }
}

void heroesGrave()
{
    prln("You come across a simple grave.");
    prln("The time-worn headstone is unmarked.");
    prln("Pay your respects? (y / n)");
    cin >> pc.inp;
    if( inpAffirm(pc.inp) )
        {prln("You calmly pay your respects."); setKarma(pc.karma, true);}
    else
        prln("You leave the grave in peace.");

    if ( rollfor( 1, 100) ){ Shadow(pc.lvl); }
}

void Thief()
{
    int stole;
    do{stole=rand()%pc.gp-(pc.gp/2);}while(stole<0);
    cout<<"\nYou start as a dark figure runs past, bumping into you.\n";
    cout<<"Your wallet feels a bit light...that scoundrel! You're missing "<<stole<<" gold!\n";
    if(game_settings.sound) mciSendString(sdCoins.c_str(), NULL, 0, NULL);
    pc.gp-=stole;
    if(!trophy_list[trophyGetID("weasel")].unlock&&rollfor(pc.karma,1000)) trophyGet("weasel", 0);
}

void Bard()
{
    int n_song = rand()%10;

    side_char bard;
    bard.initialize(false);
    bard.n = "bard";

    prln("You see "+aoran(bard.adj, false)+bard.showName("full")+" tuning "+bard.pos+" instrument by the roadside.\n\n"+bard.pers_up+" gives you a broad wink and begins to play a song.");
    cout<<"\n"<<bard.pers_up<<" plays '"<<songs[n_song].name<<"';\n"<<songs[n_song].desc;

    switch(n_song)
    {
        case 0: //Battle march
            {pc.area.echance+=20; prln("You feel an air of danger.");}
            break;
        case 1: //Seasons' hymn
            {updateSeason(true);}
            break;
        case 2: //Sunrise melody
            {day++; timen = 0; prln("The sun streaks across the sky, followed by the moon;\nA rooster crows as the sun begins to rise again.\n");}
            break;
        case 3: //Serenity
            {pc.area.echance = 0; prln("You feel safe and at ease.");}
            break;
        case 4: //Snowfall
            {curs.weath = cweath[1]; prln("It begins to snow.");}
            break;
        case 5: //Song of storms
            {curs.weath = cweath[5]; prln("It begins to rain heavily, distant flashes of thunder lighting the clouds.");}
            break;
        case 6: //Lucky cat
            {chestroll();}
            break;
        case 7: //Spring song
            {curs = seasons[0]; seasonal_changes(curs.name);}
            break;
        case 8: //Rogue's lament
            {pc.gp += roll(10); prln("You find some coins on the ground!");}
            break;
        case 9: //Starlight aria
            {timen = 8; prln("The sky fades to darkness, millions of distant stars twinkling overhead.");}
            break;
        default: prln("Nothing interesting happens.");
            break;
    }
}

void Priest()
{
    cout<<"\nAs you're walking down the road, you see a\nrobed priest drifting placidly towards you.\n\n";
    cout<<"He smiles as he sees you.\n'Ah, a fellow traveler!'\nHe digs in his pack and presents a pouch of coins.\n\n";
    cout<<"'I believe you may need this more than I, friend.\nBest of luck on your travels!'\n\n";
    int gain = roll(50);
    cout<<"You look in the pouch.\nThere's "<<gain<<" gold in here! What a nice priest!\n";
    pc.gp+=gain;
}

void Traveler()
{
    int tx; int ty;

    do {tx=roll(10); ty=roll(10);}
    while (loc[tx][ty].name==""&&loc[tx][ty].name=="River");

    area tl = loc[tx][ty];

    cout<<"\nA brightly dressed stranger in a wide-brimmed hat lopes over a nearby hill.\n";
    cout<<"Catching sight of you, he smiles broadly, waving you over energetically.\n\n";
    cout<<"'A fellow traveler! Tell me, have you been to the "<<tl.name<<" at "<<tx<<", "<<ty<<"?'\n";
    cout<<"Before you can respond, he says,\n";

    if(tl.armory) {cout<<"'There's an excellent blacksmith there.'\n";}
    if(tl.wood) {cout<<"'A lovely forest, full of old trees, mushrooms, even some herbs.\n'";}
    if(tl.water) {cout<<"'Excellent fishing around there, you know.'\n";}
    if(tl.mine) {cout<<"'I think I saw some miners setting up camp.'\n";}
    if(tl.smithy) {cout<<"'There's an abandoned smithy there, if you're the forging type.'\n";}
    if(tl.potshop) {cout<<"'There's a witch that runs a shop there, all manner of elixirs.'\n";}
    if(tl.chest!=0) {cout<<"'There's an old, ironbound chest setting there. Who knows what riches it could contain?'\n";}
    if(tl.crops) {cout<<"'Well, rumor has it, there's a good harvest this year.'\n";}
    if(tl.name=="Pasture") cout<<"'The flocks there seem to regrow their wool after no less than 3 days.\n";
    if(tl.name=="Windmill") cout<<"'Turns out the farmer doesn't object if you\ngrind some flour at the mill there.'\n";

    cout<<"\n'An interesting place to visit. But don't take my word for it!'\nAnd with this parting wisdom, the stranger lopes away, whistling merrily.\n";

    if(!trophy_list[trophyGetID("wide-brimmed hat")].unlock&&rollfor(1,100)) trophyGet("wide-brimmed hat", 0);
}

void Faerie()
{
    //There is a chance to miss the encounter if your karma + luck is lower than 10
    //Can't be missed if you've already started the quest (in case the portal disappears)
    if(!rollfor(pc.karma+pc.lck, 10)&&pc.fairy_missive<1)
    {
        cout<<"\nYou step into a clearing, and just for a moment,\nyou're struck by the most uncanny feeling of nostalgia.\nLike you just missed something...\n";
        return;
    }

    string col[4]={"red", "green", "blue", "yellow"};
    string bon[4]={"stronger", "faster", "smarter", "luckier"};
    int coln[4]={4, 2, 3, 14};
    int ch=rand()%4;

    if(game_settings.sound) mciSendString(sdMystic.c_str(), NULL, 0, NULL);

    cout<<"\nIn a clearing ahead hovers a slim figure on iridescent wings, raven locks tossed in the wind.\n";
    cout<<"A dry twig snaps underfoot and she whirls around, revealing her alien features.\n";

    if(pc.fairy_missive==1)
    {
        cout<<"\nBefore she can flee, you hold up the ribboned missive, bearing the seal of the Fay court.\nShocked, she halts, one hand halfway to her mouth.\n\nThe Fay Princess snatches the scroll and scans it quickly,\nre-reading it several times, her expression softening.\n";
        cout<<"\nAt length, she looks up. 'I cannot tell you my true name, but you may call me Dirn'r.\nThank you for bringing me this. Please, if you ever return to my mother's court, take this to her.'\n";
        cout<<"Dirn'r hands you a dazzling brooch, smiling slightly at your reaction.\n'I hope our paths cross again, mortal.'\nWhen you look up from the gem, she's gone.\n";

        prln("You inspect the brooch. It appears you can use it to return to the Fae Court with the command 'brooch'.");
        pc.fairy_missive=2;
    }
    else if(pc.fairy_missive==2)
    {
        cout<<"Princess Dirn'r grasps your hand. 'Please, deliver that brooch to my mother.'\nYou blink and she disappears.\n";
    }
    else if(pc.fairy_missive==3)
    {
        cout<<"Princess Dirn'r smiles. 'You came to see me again, eh, mortal?'\nShe flits past you, resting a hand on your shoulder. 'Here, this will help you on your journey.'\n";
        cout<<"A nimbus of "; colSet(coln[ch], col[ch]); cout<<" light flares around you, obscuring your vision.\nWhen you open your eyes, Dirn'r has vanished.\n";
        switch(ch)
        {
            case 0: pc.str+=2; break;
            case 1: pc.dex+=2; break;
            case 2: pc.intl+=2; break;
            case 3: pc.lck+=2; break;
        }
    }
    else{

        if ( rollfor(2, 3) ) {
            cout<<"She forestalls any queries with an upraised palm.\n'Mortal, we should have never crossed paths. I grant you this boon, \nthat you may never speak of my existence.'\n";
            cout<<"\nA shower of "; colSet(coln[ch], col[ch]); cout<<" sparks blinds you momentarily;\nwhen your vision clears, the faerie has vanished.\n";
            switch(ch)
            {
                case 0: pc.str++; break;
                case 1: pc.dex++; break;
                case 2: pc.intl++; break;
                case 3: pc.lck++; break;
            }
        }
        else {
            cout << "\nThe faerie hovers uncertainly nearby.\n";
            pc.area.loc_fauna = anim[12];
        }
    }

    if(!trophy_list[trophyGetID("faerie")].unlock&&rollfor(1,100)) trophyGet("faerie", 0);
}

void addPond()
{
    pc.area.desc += "\nThere is a small, mirrorlike pond nearby.\nShadowed fish swim under the surface;\nbullfrogs croak among the reeds.\n";
    pc.area.water = true;

    pc.area.flora_types.push_back(plant[16]);//Cattails

    pc.area.fauna_types.push_back(anim[13]);//Bullfrog
    pc.area.fauna_types.push_back(anim[14]);//Heron
    pc.area.fauna_types.push_back(anim[15]);//Dragonfly
}

void Archivist(string state)
{
    equipment wares[4];
    string type[3] = {"book", "scroll", "recipe"};
    int choice = 0;

    shop archivist = {
        "Wandering Archivist",
        "You approach the young woman. She blows a strand of sweaty hair out of her face and grins,\ndropping the overstuffed knapsack with a loud thud.\n",
        "'Ah, a fellow seeker of knowledge!'\n",
        "She fumbles a small almanac out of her pocket and\nstares at it for a moment, and then back at you. 'What?'\n",
        "She roots laboriously around the sack, retrieving the\nrequested item and taking your offered coin.\n",
        "She gives you a tired grin, a half-salute\nand shoulders the pack again with a mighty groan.\n",
        5, 101};
    archivist.ext_desc="an enthusiastic young woman struggling under\nthe weight of a pack overflowing with books and scrolls";
    //Add a blank item so list starts at 1
    archivist.wares.push_back(eq[0]);

    //Populate wares with random books and scrolls via item.createBook();
    for (int i = 0; i < 4; i++) {
        wares[i].create_book(true, type[ rollfz(3) ]);
        archivist.wares.push_back( wares[i] ); }


    if (state == "create") { pc.area.loc_shop = archivist; return; }
    else if (state == "create_and_run") { pc.area.loc_shop = archivist; }
    else if (state == "run") { }

    pc.area.loc_shop.showInfo();

    //Main buy loop
    do{
       pc.area.loc_shop.buyMenu(pc.gp);
       prln("Enter a number to select an item:");

        choice = pc.getInpn();

        if(choice < pc.area.loc_shop.wares_max)
        {
            if ( pc.gp < pc.area.loc_shop.wares[choice].price ) cout << "\nYou don't have enough gold!\n";
            else {
                pickup( pc.area.loc_shop.wares[choice] );
                pc.gp -= pc.area.loc_shop.wares[choice].price;
            }//has enough gold
        }
        else if(choice == pc.area.loc_shop.wares_max) { prln(pc.area.loc_shop.leave); }//chose to leave
        else prln(pc.area.loc_shop.error);
    } while (choice != pc.area.loc_shop.wares_max);
}

void randomEventGenerator()
{
    if( (pc.area.name == "Grassland" || pc.area.name == "Forest") && rollfor(1, 12) ) {prln("You notice a small pond nearby."); addPond();}

    if(rollfor(1, 10)&&!tutorial&&pc.area.is_outside)
    {
        int enc=rollfz(13);
        switch(enc)
        {
            case 0: if(flip()&&pc.area.is_outside) enchantedWell(); else {if(!m_data.lost_in_party || lost.c_type == "enemy")theLost(0); else cout<<"The "<<lost.showName("agemf")<<" remarks, 'Nearly there now, aren't we?'\n";} break;
            case 1: {if(!m_data.lost_in_party || lost.c_type == "enemy")theLost(0); else cout<<"The "<<lost.showName("agemf")<<" remarks, 'Nearly there now, aren't we?'\n";} break;
            case 2: theNeedful(); break;
            case 3: theSick(); break;
            case 4: Thief(); break;
            case 5: Traveler(); break;
            case 6: Faerie(); break;
            case 7: theWeak(); break;
            case 8: Priest(); break;
            case 9: Bard(); break;
            case 10: if(pc.area.wood) { if( flip() ) Gatherer(); else heroesGrave();} break;
            case 11: if(!pc.area.is_outside) ; break;
            case 12: if(pc.area.is_outside) enchantedWell();
                else
                {
                    int n = roll(4);
                    switch ( roll(4) ){
                        case 1: theNeedful(); break;
                        case 2: theWeak(); break;
                        case 3: theSick(); break;
                        case 4: {if(!m_data.lost_in_party || lost.c_type == "enemy")theLost(0); else cout<<"The "<<lost.showName("agemf")<<" remarks, 'Nearly there now, aren't we?'\n";} break;
                    }
                }
            break;
            default: cout<<"\nIf you're seeing this, something went wrong.\n";
        }
    }
}

void showarea()
{
	chestroll();

	if( !pc.area.herbs )
        if( pc.area.wood || pc.area.name == "Tundra" || pc.area.name == "Lava Plain" )
            if( rollfor( (10 + pc.lck), 100) ) pc.area.herbs = true;

    if( (pc.area.loc_flora.name == "toadstool" && pc.area.loc_fauna.name == "Faerie")
        || (pc.area.x == m_data.fairy_loc_x && m_data.fairy_loc_y == pc.area.y) )
    {
        m_data.fairy_loc_x = pc.area.x;
        m_data.fairy_loc_y = pc.area.y;
        cout<<"\nYou trip over something...A toadstool? And it's got tiny doors and windows?\n";
        cout<<"Enter Fairy Village?\n(1) Yes\n(2) No\n";

        pc.inp = pc.getInps();

        if( inpAffirm(pc.inp) )
            fairy_village();
    }
    string prAt="--"+pc.area.name+"--\n\n";
    cout<<"\n    "<<pc.area.x<<", "<<pc.area.y<<"\n";
	colSet(240, prAt);

	//Display area description
	//Automatic until visits > 3
	if(pc.area.visits<=3||pc.inp=="area"||pc.inp=="around")
		cout<<pc.area.desc;

    //Add a space
    cout<<"\n";

    //display companion idle
    if(pc.comp.name!="empty")
        {colSet(pc.comp.color, pc.comp.name); cout<<" is"<<pc.comp.idle[0]<<"behind you.\n";}

    //display NPCs from area.displayArea()

	//display mini-quests nearby
	if(m_data.weak_nearby) {cout<<"There is "<<aoran(weak.n, 0)<<weak.n<<" being menaced by an enemy nearby.\n";}
	if(m_data.sick_nearby) {cout<<"A "; colSet(14, "sick "); cout<<sick.showName("agemf")<<" is languishing nearby.\n";}
	if(m_data.needful_nearby) {cout<<"A "; colSet(14, "needful "); needful.showIdle("n", "n");}

	//display area specific objects by area ID
	switch(pc.area.id)
	{
		case 20:
			cout<<"An aged Elf is bent over a table, inspecting a wide array of precious stones through extremely thick glass goggles.\nA hand-painted sign declares his table a '"; colSet(15, "gemshop"); cout<<"'.\n";
		break;
	}

    //show flora, fauna, buildings and shops
	pc.area.display_env();

	//guild shops
	if(pc.area.name=="Grand Hall"&&pc.clas=="Paladin")
        {cout<<"Armskeeper Emiria is also operating the "; colSet(4, "guildshop"); cout<<" here.\n";}
    if(pc.area.name=="Arcanum"&&pc.clas=="Archon")
        {cout<<"Azelfoff is also operating the "; colSet(3, "guildshop"); cout<<" here.\n";}
    if(pc.area.name=="Temple of Light"&&pc.clas=="Avatar")
        {cout<<"Sister Suni is also operating the "; colSet(14, "guildshop"); cout<<" here.\n";}
    if(pc.area.name=="Serpents' Den"&&pc.clas=="Assassin")
        {cout<<"Hakon is also operating the "; colSet(2, "guildshop"); cout<<" here.\n";}

    //display terrain features, water, trees, etc.
    if(pc.area.water)
		cout<<"It looks like there's a good fishing spot nearby.\n";
	if(pc.area.mine)
		cout<<"You spot several ore veins among the rocks.\n";
	if(pc.area.name=="Pasture")
	{
		if(pc.area.sheep)
			cout<<"There is a herd of fluffy and content sheep milling about.\n";
		else
			cout<<"There is a herd of recently sheared sheep standing\nclose together. They look quite chilly.\n";
        cout<<"There is a solitary dairy cow, grazing near the fence.\n";
	}
	if(pc.area.crops)
		{cout<<"There are a few rows of crops that look ready for "; colSet(14, "harvest"); cout<<".\n";}

    //area specific objects by area name
    if(pc.area.name=="City Center")
        {cout<<"You navigate the crowd, your ears full of snippets of conversation.\n"; //Gossip
         cout<<"Facing the square from the northeast is an elegant "; colSet(9, "library"); cout<<".\n";}
	if(pc.area.name=="City Gate West")
		{cout<<"There is a stall set forward from the rest\nwith a crude drawing of a fish pinned to the post,\nand the words '"; colSet(15, "fishmarket"); cout<<"' scrawled roughly underneath.\n";}
    if(pc.area.name=="Village"||pc.area.name=="Elven Village"||pc.area.name=="Winterhold"||pc.area.name=="City Gate East")
        {cout<<"Dim light and the sound of laughter spill from the doorway of a nearby "; colSet(14, "inn"); cout<<".\n";}
    if(pc.area.name=="Village"||pc.area.name=="City Center")
        {cout<<"There is a communal oven nearby the road.\n";}
    if(pc.area.name=="Castle Courtyard")
        {cout<<"The castle "; colSet(3, "quartermaster"); cout<<" is training some hapless recruits nearby.\n";}
    if(pc.area.name=="Grassland Caravan")
        {cout<<"A dark-haired "; colSet(2, "fencer"); cout<<" is practicing his set nearby.\n";}
    if(pc.area.name=="Windmill")
        {cout<<"The door to the "; colSet(14, "windmill"); cout<<" hangs ajar, its broad\narms turning slowly in the breeze.\n";}
    if(pc.area.name=="Frost's Eyrie"){
        cout << "There is a narrow "; colSet(COL_ORANGE, "path"); cout << " leading up the mountain.\n";
    }

    //inherent shops
	if(pc.area.armory)
		{cout<<"To one side is a shop with the word "; colSet(2, "armory"); cout<<" painted under two crossed swords.\n";}
	if(pc.area.smithy)
		{cout<<"A short ways from the road is a "; colSet(6, "smithy"); cout<<", complete with anvil and furnace.\n";}
	if(pc.area.potshop)
		{cout<<"Set back from the path is a dark, shuttered house.\n A cauldron in cracked paint on the door bears the word '"; colSet(13, "potions"); cout<<"'.\n";}
	if(pc.area.toolshop)
		{cout<<"A traveling "; colSet(6, "merchant"); cout<<" displays an array of tools on the side of the road.\n";}

	//display chest status
	if(pc.area.chest==1)
		{cout<<"There is an old treasure "; colSet(14, "chest"); cout<<" here.\n";}
	else if(pc.area.chest==2)
		cout<<"There is an empty chest here.\n";

    //display any items on the ground
	if(pc.area.loot.name != "empty")
		{ cout << "There is " << aoranf(pc.area.loot.showName(), false) << " on the ground.\n"; }
	if(pc.area.foe.alive)
		cout << "There is " << aoranf(pc.area.foe.name, false) << " " << pc.area.foe.idle << " nearby.\n";

    //fire
	if(pc.area.fire)
		cout<<"There is a campfire crackling merrily here.\n";

    //display lost idle
    if(m_data.lost_in_party)
    {
        lost.showIdle("full", "null");
        if(pc.area.x==m_data.lostx&&pc.area.y==m_data.losty)
            theLost(1);
    }

    //Get companion action; default is idle (1/6)
    if(pc.comp.name!="empty") pc.comp.chooseAction();

    //roll for chance of random encounter
    randomEventGenerator();
}

void magshop()
{
	int num;
	string wares[9]={"", "silver ore", "gold ore", "crystal", "sapphire", "emerald", "ruby", "amethyst", "diamond"};
	int price[9];
	for (int x=1;x<9;x++)
	{
		price[x]=100+(x*120);
	}

	cout<<"The wizened old Elf peers at you wordlessly\nfor a moment, magnifying lenses making\nhis eyes appear huge. He squints, then removes the lenses.\n'Ah! A customer!And normal-sized after all. Come here, take a look!\nFine specimens, these are! Worth every coin.'\n";

	do
	{
		cout<<"'Now, which did you want?'\n";
		cout<<"Gold: "<<pc.gp<<endl;

		for(int x=1;x<9;x++)
		{
			cout<<"("<<x<<") "<<wares[x]<<": "<<price[x]<<" gold\n";
		}
		cout<<"(9) Leave\n\n";
		num=check_int();;
		if(pc.gp>=price[num]&&num<8)
		{
			cout<<"You hand the man a sack of coin, and he\nhands you the "<<wares[num]<<".\n";
			pc.gp-=price[num];
			int id=getItemID("none", wares[num]);
			pickup(eq[id]);
		}
		else if(num==9)
			cout<<"You take your leave. The old man\nreturns to inspecting his wares.\n";
		else cout<<"You can't afford that!\n";
	}while(num<9);
}

void toolshop()
{
	int price[8]={0, 10, 30, 10, 20, 20, 20, 15};
	string wares[8]={"null", "Woodcutting Axe\t", "Pickaxe\t", "Tinderbox\t", "Chisel\t", "Hammer\t", "Needle & Thread\t", "Shears\t"};

	cout<<"\nYou approach the merchant, a dark-haired,\ndark-eyed man seated cross-legged on a colorful\nrug. He gestures silently to the items spread out\nbefore him, each with a small price tag attached.\n";
	int num;

	do
	{
		cout<<"Gold: "<<pc.gp<<endl;
		prln("Tools:");
		cout<<"~+-------------------------+~\n";
		for(int x=1; x<8;x++)
		{
			cout<<"["<<x<<"] ---\t| "<<wares[x]<<" \t| "<<price[x]<<" gold\n";
		}
		cout<<"[8] --- leave\n\n";

		cout<<"Enter a number: \n";
		num=check_int();


		if(pc.gp>=price[num]&&num<8)
		{
		pc.gp-=price[num];
		switch(num)
			{
				case 1: pc.axe = true; break;
				case 2: pc.pick = true; break;
				case 3: pc.tbox = true; break;
				case 4: pc.chis = true; break;
				case 5: pc.ham = true; break;
				case 6: pc.need = true; break;
				case 7: pc.shears = true; break;
				default: cout << "The merchant shakes his head, not understanding.\n";
			}
			cout<<"You hand the vendor some coins and\nput the "<<wares[num]<<" in your bag.\n\n";
			}
			else if(num==8)
			{
				cout<<"The merchant gives you a curt nod as you turn to leave.\n";
			}
			else
			{
				cout<<"The merchant shakes his head quizzically.\n";
			}
	}while(num!=8);
}

void armory()
{
	int num;
	equipment wares[4];
	switch(pc.lvl)
	{
		case 1 ... 5:
		{
			wares[1]=eq[3]; //Iron dagger
			wares[2]=eq[17]; //Leather tunic
			wares[3]=eq[22]; //Wooden charm
		}
		break;
		case 6 ... 10:
		{
			wares[1]=eq[getItemID("iron", "chainmail")];
            wares[2]=eq[getItemID("iron", "shortsword")];
            wares[3]=eq[getItemID("iron", "small shield")];
		}
		break;
		default:
        {
            wares[1]=eq[getItemID("steel", "chainmail")];
            wares[2]=eq[getItemID("steel", "longsword")];
            wares[3]=eq[getItemID("steel", "small shield")];
        }
        break;
	}

    colSet(240, "Armory");
	cout<<"\nThe sharp, bittersweet aroma of pipe-leaf fills your\nnostrils as you enter the dimly lit armory. A tall, \nburly man with a long mustache greets you heartily from \nbehind longer eyebrows, a battered wooden pipe \nclenched between his teeth.\n'Ho there, stranger! Feel free to browse me wares.'\n\n";

do
{
	cout<<"'What'll it be?'\n";
	cout<<"Gold: "<<pc.gp<<endl;
	cout<<"(buy) purchase an item\n(sell) sell an item from your inventory\n(leave) exit the shop\n";
	cin>>pc.inp;
	if(pc.inp=="buy")
	{
	    cout <<"\n\n  Item\t\t\t|  Price\n";
	    cout <<"+-----------------------+-------------+\n";

		for(int x=1;x<4;x++)
		{
			cout<<"["<<x<<"] --- "<<wares[x].mat<<" "<<wares[x].name<<"\t| "<<wares[x].price<<" gp\n";
		}
		cout<<"\nType the number of the item you want to buy, or\n'4' to return.\n";
			num=check_int();;
			if(pc.gp>=wares[num].price&&num!=4)
			{
				cout<<"\n'Excellent choice!'\n";
				pickup(wares[num]);
				pc.gp-=wares[num].price;
			}
		else if(pc.gp<wares[num].price&&num!=4)
			cout<<"\n'What are yeh, some sort o' joker? Ye can't afford that!'\n";
	}
	else if(pc.inp=="sell")
	{
	    pc.showInv();
		cout<<"\nEnter a number to sell an item, or '11' to return.\n";
		num=check_int();
		if(num==11) return;
		if(pc.inv[num].name!="empty")
		{
			if(pc.inv[num].type=="quest")
			cout<<"You can't sell that!\n";
			else
			{
			cout<<"The shopkeeper looks the item over and nods approvingly.\nHe hands you a small pouch of coins.\n\n";
			pc.gp+=pc.inv[num].price;
			pc.carrywt-=pc.inv[num].wt;
			pc.inv[num]=eq[0];
			}
		}
		else
			cout<<"\nThe shopkeeper guffaws loudly.\n 'Sure, but yer not the first to try the ol'\n'invisible sword' trick.\n I'll take me chances with loot I can see, thanks!'\n";
	}
	else if(pc.inp=="leave")
		cout<<"\n'Pleasure doin' business.'\n";
	else
		cout<<"\n'Eh? You speakin' some kinda foreigner?'\n";
}while(pc.inp!="leave");
}

void potshop()
{
    colSet(240, "Potion Shop");
	cout<<"\nYour eyes take a moment to adjust to the dim,\nflickering candlelight. Shadows dance in\nthe corners of the room, and the air is thick\nwith a multitude of pungent vapors. An old woman\nsits hunched on a stool behind a pair of mixing\nvats; one contains a bright red liquid, the\nother bright blue. Both are steaming slightly.\nNext to a mortar and pestle on a low table is a\njar of green liquid.\n\n";

	cout<<"\nThe woman in the pointed hat grins a\ncrooked grin, waving a bony hand at a shelf\nof glass bottles.\n";

	int num;
	string pot[5]={"", "red", "blue", "green", "clear"};
	int price[5]={0, 20, 15, 40, 100};

	do
	{
	cout<<"'What'll it be, dearie?'\n";
	cout<<"Gold: "<<pc.gp<<endl;
	cout<<"1: Health Potion - 20 gp\n2: Mana Potion - 15 gp\n3: Antidote - 40 gp\n4: Clear potion? - 100 gp\n5: Leave\n";
	num=check_int();;
		if(pc.gp>=price[num]&&num!=5)
		{
			pc.gp-=price[num];
			cout<<"You uneasily purchase a bottle of "<<pot[num]<<" potion.\n";
			if(num==1) pc.hpot++;
			else if(num==2) pc.mpot++;
			else if(num==3) pc.apot++;
			else if(num==4) {pickup(eq[64]); cout<<"Oh...it's just an empty bottle? Huh.\n";}
		}
		else if(pc.gp<price[num]&&num!=5)
			cout<<"She slaps your hand as you reach\nfor the bottle. 'Not without gold, you don't!'\n";

		else if(num==5)
			cout<<"'Come again soon!' the old woman cackles as you leave.\n";
		else cout<<"'The old crone cups her hand to her ear. 'What's that?'\n";
	}while(num!=5);
}

void quartermaster()
{
    int wares[3]={0, 14, 30}; //Iron longsword, plate armor, sheath
    cout<<"\nThe heavyset veteran looks up with a grin as you approach, blocking a wild strike and shoving the frustrated trainee.\n";
    cout<<"\n'Enough now, lads! We've got company. At ease!'\nHe removes his helmet, washing his face at a nearby cask.\n";
    cout<<"\n'What can I do for you, stranger? Come to join up, eh?'\n";
    cout<<"(1) Buy\n(2) Train\n(3) Leave\n";
    cin>>pc.inp;
    if(pc.inp=="1")
    {
        int num;
        int x;
        for(x=1;x<3; x++)
        {
            cout<<"("<<x<<") "<<eq[wares[x]].mat<<" "<<eq[wares[x]].name<<" - "<<eq[wares[x]].price<<"gp\n";
        }
        cout<<"(3) Leather sheath - 45gp\n(4) Leave\n";
        num=check_int();;
        if(num>3) return;
        if(num==3)
        {
            if(pc.gp<45){cout<<"\n'You haven't got the gold for that!'\n"; return;}
            if(pc.sheath){cout<<"\n'Are ye daft? You already have a sheath on your belt!'\n"; return;}
            else
            {
                cout<<"\nHe hands you a sturdy leather sheath. You take a moment to attach it to your belt.\n";
                pc.gp-=45;
                pc.sheath=true;
            }
            return;
        }
        if(pc.gp>=eq[wares[num]].price&&num<5)
        {
            pc.gp-=eq[wares[num]].price;
            cout<<"\nThe old soldier nods, taking the item off a rack and handing it to you.\n";
            pickup(eq[wares[num]]);
        }
        else cout<<"'You haven't got the gold for that!'\n";
    }
    else if(pc.inp=="2")
    {
        cout<<"\nThe grizzled old soldier chuckles dryly. 'Aye, we can teach you a thing or two. Doesn't come cheap, though - 100 gold.'\n";
        cout<<"'What'll it be?'\n";
        cout<<"(1) Strength training\n(2) Agility course\n(3) Combat training\n(4) Leave\n";
        cin>>pc.inp;
        if (pc.gp<100){cout<<"He laughs. 'Sorry, stranger, come back when ye've got the coin.'\n"; return;}
        if(pc.inp=="1")
        {
            if(pc.str>9) {cout<<"\nHe shakes his head, looking you up and down. 'Afraid there's nothing I can teach you.'\n"; return;}
            cout<<"\nThe quartermaster sets you to lifting buckets full of sand,\nsparring with his recruits while you strain against the heavy weight.\n";
            cout<<"\nJust as your arms begin to give, he calls out\n'That's enough for today, stranger. Good work.\n'";
            cout<<"\nYour arms are weak and sore, but you feel a little stronger all the same.\n";
            pc.str++;
        }
        else if(pc.inp=="2")
        {
            if(pc.dex>9) {cout<<"\nHe shakes his head, looking you up and down. 'Afraid there's nothing I can teach you.'\n"; return;}
            cout<<"\nThe quartermaster has you run an obstacle course. The group of recruits pauses to watch,\nlaughing uproariously as you struggle with rolling logs and rope swings.\n";
            cout<<"\nYour pride is slightly injured, but you feel a little faster than before.\n";
            pc.dex++;
        }
        else if(pc.inp=="3")
        {
            cout<<"\nThe burly old soldier looks at you for a moment, then shrugs and nods to one of his men.\n";
            cout<<"\nThe recruit unlocks a nearby cage, releasing the snarling wolf within...\n";
            enem temp = elist[3];
            cout << "\nDebug: set temporary enemy\n";
            pc.area.foe = temp;
            cout << "\nDebug: set area enemy\n";
            pc.area.foe.alive = true;
            int xp_before = pc.xp;
            combat();
            if ( pc.xp != xp_before ) {
                int gain=roll(4);
                if(admin) cout<<"Chance to gain stat point ->"<<gain<<"\n";
            if(gain==1)
            {
                int gaint=rand()%2;
                if(gaint==0&&pc.atk<11)
                {
                    pc.atk++;
                    cout<<"You gain a little skill with your weapon.\n";
                }
                if(gaint==1&&pc.def<11)
                {
                    pc.def++;
                    cout<<"You gain a little skill in blocking and defending.\n";
                }
            }}
        }
        pc.gp-=100;
    }
    else cout<<"The quarter master waves you on, turning back to his trainees. 'Alright, ya lily-livered sons'o she-goats, who's next?'\n";

}

void fencer()
{
    cout<<"\nThe fencer pretends to sheath his prop weapon, holding it at his side as you approach.\n\n'Hello!,' he says with a flourish. 'What can I do for you?'\n\n";
    cout<<"(1) buy\n(2) leave\n";
    cin>>pc.inp;
    if(pc.inp=="1")
    {
        int ware[5]={0, 95, 96, 97, 98};
        for(int x=1;x<5;x++)
        {

            cout<<"("<<x<<") "<<eq[ware[x]].mat<<" "<<eq[ware[x]].name<<" - "<<eq[ware[x]].price<<"gp\n";
        }
        int num;
        cout<<"(5) Leave\n";
        cout<<"Buy which?\n";
        num=check_int();;

        if(num!=5&&pc.gp>=eq[ware[num]].price)
        {
            pc.gp-=eq[ware[num]].price;
            pickup(eq[ware[num]]);
        }
        else {cout<<"The actor gives you a mock salute and returns to honing his skills.\n"; return;}
    }
}

void fishop()
{
	string wares[3]={"Fishing Rod", "Bait(5)", "Feathered Lure"};
	int prices[3]={20, 15, 50};
	int num;

    colSet(240, "Fish Shop");
	cout<<"\n\nYou stop at one of the friendlier looking market stalls.\nA fisherman with narrow eyes, a narrow moustache, and a knotted rope headband grins at you,\nwaving a hand around the shop.\n";
	do
	{
		cout<<"'Whatcha need? Or are ya here to sell some fish?'\n";
		cout<<"Gold: "<<pc.gp<<endl;
		cout<<" (1) Buy fishing gear\n (2) Sell fish\n (3) Leave\n";
		num=check_int();

		switch(num)
		{
			case 1://buy
			{
				cout<<"'Well, which one do you want?'\n";
				for(int x=0;x<3;x++) { cout<<x<<": "<<wares[x]<<"- "<<prices[x]<<" gp\n"; }
				num=check_int();

				if(pc.gp<prices[num]) { cout<<"'You can't afford that!'\n"; }
				else { pc.gp-=prices[num];
				switch(num)
				{
					case 0: { pc.frod=1; } break;
					case 1: { pc.bait+=5; } break;
					case 2: { pc.frod=2; } break;
				}
			}
			}
			break;
			case 2://sell
			{
				pc.showInv();;
				cout<<"'So, what'd you want to sell me?'\n";
				num=check_int();;
				if(pc.inv[num].name=="fish")
				{
				    int val=pc.inv[num].wt*10;
					cout<<"'Whoa, a "<<pc.inv[num].ench<<"? And it weighs "<<pc.inv[num].wt<<" pounds?! I'll take it!'\n";
					pc.carrywt-=pc.inv[num].wt;
					cout<<"He hands you "<<val<<" coins.\n";
					pc.gp+=val;
					pc.inv[num]=eq[0];
				}
				else
					cout<<"The fisherman gives you a derisive look.\n'This is a fishing stall. I don't buy anything but fish.\n'ttaku...'";
			}
			break;
		}
	}while(num!=3);
}

void guildshop(int guild)
{
	string name[4]={"Emiria", "Hakon", "Azelfoff", "Suni"};
	string meet[4]={
		"Emiria, Paladin armskeeper, greets you with a stoic nod.\nClad in full enameled plate armor, minus helmet and shield,\nshe stands stiffly beside several racks of equipment.", "Hakon sizes you up with a disdainful glance as you approach, then turns back to the makeshift bar.\nHe speaks without looking at you, his wares concealed in a crate under the bar.", "Azelfoff the Wizened waves emphatically as you approach.\nHis wild hair and beard, archaic blue robes, pointed hat, and enthusiastic attitude\nset him sharply apart from the brooding Arcanum mages.", "Suni is a beatific Priestess of Light, aging but youthful in spirit.\nShe wears traditional plain white robes and a crisp white head covering.\nShe smiles widely when she speaks, radiating serenity and joy."};
	string greet[4]={
		"'Here's what I have:'", "'Don't tell anyone where you got this.'", "'Would you like to spit fire?\n ...no? Hail, perhaps?'", "'All proceeds go to the temple!'"};
	string bought[4]={"The tall Paladin wordlessly takes the offered coins and hands you the item from a rack.", "Hakon leans down and exchanges the item for your gold under the table.", "Azelfoff materializes the requested\nitem with a loud 'pop', swiping the coins\nfrom your palm.", "From somewhere under her habit, Sister Suni\npulls the item, placing it in your hand and\npocketing the gold."};
	string bye[4]={"Emiria nods again as you turn to leave.", "The big mercenary grunts as you stand up, but\notherwise takes no notice.", "'Between you and me,' says Azelfoff, \n'I could care less about all this mumbo jumbo;\nall I know is, magic's good money! Hee hee!'", "'Go smash up some baddies for me!\n......\nPeace and mercy in the Light~!'"};
	string what[4]={"Emeria looks at you questioningly.", "'Huh?'", "Azelfoff scratches his head.\n'No, I don't think I know that spell.'", "'I'm sorry, I don't understand!'"};
	equipment wares[4];

	switch(guild)
	{
		case 0:
		{
			wares[0]=eq[280];//Paladin sword
			wares[1]=eq[281];//paladin plate
			wares[2]=eq[282];//paladin shield
			wares[4]=eq[279];//crimson cape
		}
		break;
		case 1:
		{
			wares[0]=eq[286];//assassin mask
			wares[1]=eq[285];//shadowed leather
			wares[2]=eq[287];//ebony katana
			wares[3]=eq[288];//assassin's poison
		}
		break;
		case 2:
		{
			wares[0]=eq[290];//arcanum robes
			wares[1]=eq[273];//arcanum hood
			wares[2]=eq[291];//arcanum staff
			wares[3]=eq[274];//iron bangle
		}
		break;
		case 3:
		{
			wares[0]=eq[277];//light robes
			wares[1]=eq[278];//book of light
			wares[2]=eq[276];//golden bangle
			wares[3]=eq[272];//blessed coif
		}
		break;
	}

	cout<<meet[guild]<<endl;
	int num;
	do
	{
	    //Display shop wares
		cout<<greet[guild]<<endl;
		for (int x=0;x<4;x++)
		{
			cout << "[" << x+1 << "]: " << wares[x].showName() << " ------- "<<wares[x].price<<" gold\n";
		}
		cout<<"[5]: Leave\n";

		//Get input
		cout<<"Enter a number: \n";
		num=check_int();
		//Adjust to avoid zero input
		num -= 1;

		if(num<5){
		if(pc.gp>=wares[num].price)
		{
			cout<<bought[guild]<<endl;
			pc.gp-=wares[num].price;
			pickup(wares[num]);
		}
		else cout<<"\nNot enough gold!\n";
		}
		else if(num==5)
			cout<<bye[guild]<<endl;
		else cout<<what[guild]<<endl;
	}while(num!=5);
}

void move()
{
	bool moving=false;
	int newx;
	int newy;

	do
	{
		cout<<"Move in which direction?\n";
		if(pc.area.north==true)
			cout<<"(n) North\n";
		if(pc.area.south==true)
			cout<<"(s) South\n";
		if(pc.area.east==true)
			cout<<"(e) East\n";
		if(pc.area.west==true)
			cout<<"(w) West\n";

		if (pc.inp != "north" && pc.inp != "south" && pc.inp != "east" && pc.inp != "west" && pc.inp != "n" && pc.inp != "s" && pc.inp != "e" && pc.inp != "w") pc.inp = pc.getInps();

		if((pc.inp=="n"||pc.inp=="north")&&pc.area.north==true)
		{
			newx=pc.area.x;
			newy=pc.area.y-1;
			moving=true;
		}
		else if((pc.inp=="s"||pc.inp=="south")&&pc.area.south==true)
		{
			newx=pc.area.x;
			newy=pc.area.y+1;
			moving=true;
		}
		else if((pc.inp=="e"||pc.inp=="east")&&pc.area.east==true)
		{
			newx=pc.area.x+1;
			newy=pc.area.y;
			moving=true;
		}
		else if((pc.inp=="w"||pc.inp=="west")&&pc.area.west==true)
		{
			newx=pc.area.x-1;
			newy=pc.area.y;
			moving=true;
		}
		else
		{
			cout<<"You can't go that way!\n";
			return;
		}
		if(loc[newx][newy].name==""||(tutorial&&tut[newx][newy].name=="null"))
		{
			moving=false;
			cout<<"You attempt to step forward but are repulsed\nby some sort of invisible barrier.\n";
		}
		//Update player area
		if(moving==true)
		{
		    if(!tutorial){
                loc[pc.area.x][pc.area.y].lastvisitday=day;
                pc.area=loc[newx][newy];
                loc[newx][newy].visits++;}
			else{
                tut[pc.area.x][pc.area.y].lastvisitday=day;
                pc.area=tut[newx][newy];
                tut[newx][newy].visits++;
			}
			if(pc.status=="Terrifying") pc.area.echance=0;

            pc.area.load_flora();
            /**debugMsg("Load flora", 1);*/

            /*--------------==================>> BUG <(_)> in pc.area.load_flora() */

            pc.area.load_fauna();
            /**debugMsg("Load fauna", 1);*/

            if(rollfor(1, 100)&&pc.area.loc_shop.name=="null") pc.area.loc_shop=shops[0];
            /**debugMsg("rolled for shop encounter", 1);*/

            if(pc.area.wood&&rollfor(10, 100)) pc.area.loc_shop=shops[5];
            /**debugMsg("rolled for shop encounter", 2);*/

            if ( rollfor(1, 32) )
            {
                if (pc.area.loc_shop.name == "null") Archivist("create");
            }

            m_data.needful_nearby=false;
            m_data.sick_nearby=false;

            //Exploration trophies
            if(!trophy_list[trophyGetID("mountain")].unlock&&rollfor(1,100)&&pc.area.name=="Glacial Palace") trophyGet("mountain", 0);
            if(!trophy_list[trophyGetID("throne")].unlock&&rollfor(1,100)&&pc.area.name=="Throne Room") trophyGet("throne", 0);
            if(!trophy_list[trophyGetID("redwood")].unlock&&rollfor(1,100)&&pc.area.name=="Sacred Forest Grove") trophyGet("redwood", 0);
            if(!trophy_list[trophyGetID("gear")].unlock&&rollfor(1,100)&&pc.area.name=="Elven Workshop") trophyGet("gear", 0);
            //if(!trophy_list[trophyGetID("")].unlock&&rollfor(1,100)&&pc.area.name=="Glacial Palace") trophyGet("", 0);

			string stepr[3]={"0 to 1100", "1101 to 2200", "2201 to 3300"};
			string sdStep="play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\step2.mp3 from "+stepr[rand()%3];
            if(game_settings.sound) mciSendString(sdStep.c_str(), NULL, 0, NULL);

			if(game_settings.display_type==0) showarea();
			else if(game_settings.display_type==1) show_map();

			//Exploration triggers

			//Grognak
			//Frost
			//Aldo
			//Luz
			if(pc.area.name=="Ancient Forest"&&pc.area.visits<=1&&pc.comp.name=="Luz") pc.comp.infos(4);

			//Misc
			if (pc.area.name == "City Center") {pc.area.npcs[0] = cast_npc[0];}
		}

	}while(moving!=true);
	moving=false;

	//Save game on travel
	autosave();
}

void drop()
{
	int num;
	pc.showInv();

	cout<<"Drop item from which slot?\n";
    num = pc.getInpn();

    if(pc.inv[num].type == "quest") {prln("You can't drop that! It's too important!"); return;}

    if(pc.area.loot.id == 0)
    {
        cout << "\nYou drop the "<<pc.inv[num].name<<".\nIt lands on the ground.\n";
        pc.area.loot = pc.inv[num];
        pc.inv[num]=eq[0];
    }
    else
    {
        prln("There is already an item on the ground here...\nIf you drop another item it will disappear.");
        prln("Replace the "+pc.area.loot.showName()+"?");
        if( inpAffirm( pc.getInps() ) )
        {
            cout << "\nYou drop the " << pc.inv[num].name << ".\nThe " << pc.area.loot.name << " clatters into a gully, never to be seen again.\n";
            pc.area.loot = pc.inv[num];
            pc.inv[num]=eq[0];
        }
        else {
            prln("Swap your item for the "+pc.area.loot.showName()+"?");
            if( inpAffirm( pc.getInps() ) ){
                equipment temp = pc.area.loot;
                pc.area.loot = pc.inv[num];
                pc.inv[num] = temp;
            }
            else prln("You leave the item where it is.");
        }
    }

    if(game_settings.sound) mciSendString(sdItem.c_str(), NULL, 0, NULL);
}

void customize()
{
	cout<<"Please, tell me about yourself. First of all...\n";
	cout<<"\nWhat is your gender?\n(enter 'none' to skip any question.)\n";
	pc.sex = pc.getInps();
	cout<<"\nHow many years have you walked these lands?\n";
	pc.age = pc.getInpn();
	cout<<"\nHow tall are you?\n";
	pc.height = pc.getInpn();
	cout<<"\nPlease excuse me for asking this, but,\nHow much do you weigh?\n";
	pc.wt = pc.getInpn();

	//Set eye, hair, and skin color, as well as voice and attitude
	pc.setAttributes();

	cout<<"Almost done!\n";
	cout<<"What is your name?\n";
	pc.name = pc.getInps();

	cout << "\nLooks great!\n";
	cout<<"\n -> You can change your character details at any time\n   with the 'customize' or 'setattributes' commands.\n";
}

void randomize()
{
    string eyec[8]={"blue", "green", "brown", "hazel", "black", "silver", "golden", "crimson"};
    string hairc[8]={"blonde", "auburn", "tawny", "brown", "raven", "silver", "copper", "no"};
    string buildm[4]={"scrawny", "lean", "muscular", "heavy"};
    string buildf[4]={"slender", "lithe", "athletic", "curvacious"};
    string sex[2]={"male", "female"};
    string npref[10]={"Fal", "Gar", "Sen", "Ar", "Dir", "El", "Isth", "Far", "Qu", "Ant"};
    string nsuf[10]={"iel", "anon", "ius", "ias", "eth", "anel", "edor", "nes", "nine", "en"};
    string skinc[7]={"pale", "tawny", "dark", "rosy", "tan", "brown", "fair",};
    string races[4] = {"Elven", "Dwarven", "Human", "Merfolk"};
    int npn, nsn, buildt;

    do
    {
        npn=roll(10)-1;
        nsn=roll(10)-1;
        pc.name=npref[npn]+nsuf[nsn];

        pc.eyec=eyec[rand()%8];
        pc.hairc=hairc[rand()%8];
        pc.skinc=skinc[rand()%7];
        pc.sex=sex[rand()%2];
        pc.race = races[ rollfz(4) ];
        buildt=rand()%4;
        if(pc.sex=="male") pc.build=buildm[buildt];
        else pc.build=buildf[buildt];
        pc.height=rand()%180+120; if(pc.race=="Elven") pc.height+=rand()%20+10; if(pc.race=="Dwarven") pc.height-=rand()%20+10;
        pc.wt=rand()%((pc.height/2)+(buildt*5))+120;
        pc.age=rand()%32+18;
        cout<<pc.name<<":";
        cout<<"\n is a "<<pc.age<<" year old "<<pc.race<<" "<<pc.sex<<" with a "<<pc.build<<" body,\n"<<pc.hairc<<" hair, "<<pc.eyec<<" eyes, and "<<pc.skinc<<" skin.\n"<<pc.name<<" stands "<<pc.height<<" cm tall and weighs "<<pc.wt<<" pounds.\n";
        cout<<"Keep this character?\n(y) Save and continue\n(n) Randomize traits\n";
        cin>>pc.inp;
    }while(pc.inp!="y");
}

void chars()
{
    pc.showChar();
}

void race()
{
	int num;
		do
		{
			cout<<"\nChoose a race: \n\n";
			cout<<"(1): Elven\n";
			cout<<"(2): Dwarven\n";
			cout<<"(3): Human\n";
			num=check_int();;
			switch(num)
			{
				case 1:
				{
					cout<<"\nThe Elves are an agile and clever forest race,\npreferring to master their crafts in relative\nsolitude amongst the trees.\n\n-Elves are more dextrous than other races\n-Elves are slightly more skilled in sewing, carving, and enchanting.\n-Elves often carry tools for sewing and carving.\n";
					cout<<"\nThis is your race? (y/n)\n";
					cin>>pc.inp;
					if(pc.inp=="y")
					{
					pc.race="Elf";
					pc.swlvl+=2;
					pc.clvl+=2;
					pc.enchlvl++;
					pc.dex++;
					pc.need=true;
					pc.chis=true;
					}
				}
				break;
				case 2:
				{
					cout<<"\nThough the Dwarves do not have a permanent\n settlement on the surface, this hardy\nand fiery-spirited race will often be found camped\nnear mines and mountains.\n\n-Known for their stalwart nature, Dwarves cannot get drunk from ale\n-Dwarves are slightly stronger than other races\n-Dwarves are skilled in smithing, mining, and carpentry.\n-Dwarves carry tools for mining and smithing\n";
					cout<<"\nThis is your race? (y/n)\n";
					cin>>pc.inp;
					if(pc.inp=="y")
					{
					pc.race="Dwarf";
					pc.smlvl+=2;
					pc.mlvl+=2;
					pc.str++;
					pc.pick=true;
					pc.ham=true;
					}
				}
				break;
				case 3:
				{
					cout<<"\nThe most populous of the three races, Humans are known for their\ninventiveness, adaptability, and diverse culture.\n\n-Humans have a slightly higher intellect than other races\n-Humans are slightly more skilled in woodcutting, fishing, and cooking\n-Humans carry a wood axe and fishing rod\n";
					cout<<"\nThis is your race? (y/n)\n";
					cin>>pc.inp;
					if(pc.inp=="y")
					{
                        pc.race="Human";
                        pc.wclvl+=2;
                        pc.flvl+=2;
                        pc.cklvl++;
                        pc.intl++;
                        pc.frod=1;
                        pc.bait+=5;
                        pc.axe=true;
					}
				}
				break;
			}
		}while(pc.inp!="y");
}

void select_class()
{
	do
	{
	cout<<"Choose a starting class:\n\n";
	cout<<"(1) "; colSet(4, "Adventurer"); cout<<"\n";
	cout<<"(2) "; colSet(9, "Scholar"); cout<<"\n";
	cout<<"(3) "; colSet(2, "Wanderer"); cout<<"\n";
	cin>>pc.inp;
	if(pc.inp=="1")
	{
	cout<<"\nRanging the wilderness in search of fortune and glory,\nthe Adventurer leaves no stone unturned, and never\nbacks down from a challenge. The Adventurer boasts\nhigher strength, speed, and luck than others,\nas well as decent survival skills.\n\n+1 Str\n+1 Dex\n+1 Lck\n+1 Firemaking + Tinderbox\nFirst Skill: Throw\n";

	cout<<"(yes) Choose this class\n";
	cout<<"(no) Go back\n";
	cin>>pc.inp;
	if(pc.inp=="yes")
	{
		pc.clas="Adventurer";
		pc.fmlvl++;
		pc.tbox=true;
		pc.str+=1;
		pc.dex+=1;
		pc.lck+=1;
	}
	}
	else if(pc.inp=="2")
	{
	cout<<"\nEver desirous of greater knowledge, The Scholar \ntakes a calculated approach to adventure, applying\na wealth of intellect in any given situation.\nAdditionally, having thought ahead, the Scholar\nhas prepared with two extra potions.\n\n+3 Int\n+1 Health Potion\n+1 Mana potion\nFirst Skill: Assess\n";

	cout<<"(yes) Choose this class.\n";
	cout<<"(no) Go back.\n";
	cin>>pc.inp;
	if(pc.inp=="yes")
	{
		pc.clas="Scholar";
		pc.intl+=3;
		pc.hpot++;
		pc.mpot++;
	}
	}
	if(pc.inp=="3")
	{
	cout<<"\nThe wayward Wanderer never stays in one place for long, getting by on fortune and quick hands.\nUsed to the rigors of travel, the Wanderer also carries food, drink and coin for the road,\nas well as a wooden flute for entertainment.\n\n+1 Dex\n+2 Lck\n+15 gold\n+Cooked meat & glass bottle of Ale\nFirst Skill: charm\n";

	cout<<"(yes) Choose this class\n";
	cout<<"(no) Go back\n";
	cin>>pc.inp;
	if(pc.inp=="yes")
	{
		pc.inv[2]=eq[43];
		pc.inv[3] = eq[getItemID("woolen", "cloak")];
		pc.inv[4]=eq[64];
		pc.inv[4].ench="ale";
		pc.gp+=15;
		pc.clas="Wanderer";
		pc.dex+=1;
		pc.lck+=2;
	}
	}
	}while(pc.inp!="yes");
}

void new_character(bool random)
{
    race();
    select_class();
    if(random) randomize();
    else customize();
}

void fully_customize(bool random)
{
    int statpts=4;
    int toolpts=2;
    int skpts=5;
    int inp=0;

    do{//restarts the whole sequence if player is unsatisfied at end

    cout<<"\nFully customize: Create your own character from scratch -\n\n";

    do{//distribute 4 stat points - 3 from class, one from race
        if(!random){
       cout<<"Stat points remaining: "<<statpts;
       cout<<"\nAdd a point to: \n(1) Strength\n(2) Dexterity\n(3) Intellect\n(4) Luck\n";
       inp=check_int();}
       else inp=rand()%4+1;

       switch(inp)
       {
       case 1:
           {pc.str++;
            statpts--;}
        break;
       case 2:
           {pc.dex++;
           statpts--;}
        break;
       case 3:
           {pc.intl++;
           statpts--;}
        break;
       case 4:
           {pc.lck++;
            statpts--;}
        break;
       default:
           cout<<"Please choose one of the following:\n";
        break;
       }
    }while(statpts>0);

    if(!random){
    cout<<"Name your race (does not affect gameplay):\n";
    cin>>pc.race;}
    else
    {
        pc.race=races[rand()%8].name;
    }

    cout<<"What sort of tools do you carry?\n";
    do
    {
        if(!random){//random flag skips display/input
        cout<<"Tools remaining: "<<toolpts<<"\n";
        cout<<"(1) Woodcutting axe\n(2) Chisel\n(3) Hammer\n(4) Pickaxe\n(5) Fishing rod & bait\n(6) Shears\n(7) Tinderbox\n(8) Needle & thread\n";
        inp=check_int();}
        else inp=rand()%8;
        switch(inp)
        {
        case 1:
            {
                pc.axe=true;
                toolpts--;
            }
            break;
         case 2:
            {
                pc.chis=true;
                toolpts--;
            }
            break;
        case 3:
            {
                pc.ham=true;
                toolpts--;
            }
            break;
        case 4:
            {
                pc.pick=true;
                toolpts--;
            }
            break;
        case 5:
            {
                pc.frod=1;
                pc.bait+=5;
                toolpts--;
            }
            break;
        case 6:
            {
                pc.shears=true;
                toolpts--;
            }
            break;
        case 7:
            {
                pc.tbox=true;
                toolpts--;
            }
            break;
        case 8:
            {
                pc.need=true;
                toolpts--;
            }
            break;
        default:
            cout<<"Please choose one of the following:\n";
            break;
        }
    }while(toolpts>0);

    if(!random) cout<<"What skills have you learned?\n";

    do
    {//every character gets two points each in two skills, and one in another (?)
        if(!random){
        cout<<"Skill points remaining: "<<skpts<<"\n";
        cout<<"(1) Woodcutting\n(2) Woodworking\n(3) Smithing\n(4) Mining\n(5) Fishing\n(6) Firemaking\n(7) Sewing\n(8) Enchanting\n(9) Hunting\n(10) Cooking\n";
        inp=check_int();}

        else inp=rand()%10+1;//if character is random, skip display/input and randomize each skill point

        switch(inp)
        {
        case 1:
            {
                pc.wclvl++;
                skpts--;
            }
            break;
         case 2:
            {
                pc.clvl++;
                skpts--;
            }
            break;
        case 3:
            {
                pc.smlvl++;
                skpts--;
            }
            break;
        case 4:
            {
                pc.mlvl++;
                skpts--;
            }
            break;
        case 5:
            {
                pc.flvl++;
                skpts--;
            }
            break;
        case 6:
            {
                pc.fmlvl++;
                skpts--;
            }
            break;
        case 7:
            {
                pc.swlvl++;
                skpts--;
            }
            break;
        case 8:
            {
                pc.enchlvl++;
                skpts--;
            }
            break;
        case 9:
            {
                pc.hlvl++;
                skpts--;
            }
            break;
        case 10:
            {
                pc.cklvl++;
                skpts--;
            }
            break;
        default:
            cout<<"Please choose one of the following:\n";
            break;
        }
    } while(skpts>0);

    if(!random){
    cout<<"What's your trade, or class?\n(Default classes are Adventurer, Scholar, or Wanderer.)\n(Custom classes will not follow class progression or learn new skills.)\n";
    cin>>pc.clas;}
    else
    {
        string clas[3]={"Adventurer", "Wanderer", "Scholar"};
        pc.clas=clas[rand()%3];
    }

    if(random) randomize();
    else customize();

    //show completed character and ask for confirmation
    pc.statscrn();
    cout<<"Enter any key to continue.\n";
    cin>>pc.inp;
    pc.showSkills();
    cout<<"\nEnter any key to continue.\n";
    cin>>pc.inp;
    cout<<"\n";
    show_tools();
    cout<<"\nAre you satisfied with this character?\n(yes) Save and continue\n(no) Restart\n";
    cin>>pc.inp;
    }while(pc.inp!="yes");
}

bool menu_return=false;

void char_menu()
{
    int inp;
    menu nc_menu={"\n\n[ New Character ]\n\n\n", "Enter a number to select an option:\n", 5};
    nc_menu.options.push_back(" ");
    nc_menu.options.push_back("New Character");
    nc_menu.options.push_back("Random Appearance");
    nc_menu.options.push_back("Full Custom");
    nc_menu.options.push_back("Full Random");

    do{
        nc_menu.display();
        inp=check_int();
        switch(inp)
        {
            case 1:
                {new_character(false); return;}
            break;
            case 2:
                {new_character(true); return;}
            break;
            case 3:
                {fully_customize(false); return;}
            break;
            case 4:
                {fully_customize(true); return;}
            break;
            case 5:
                {menu_return=true;}
            break;
            default:
                cout<<"Input error\n";
            break;
        }
    }while(inp!=nc_menu.options_limit);
}

void main_menu()
{
    int inp;
    menu mmenu={
        "\nM A I N   M E N U\n\n\n",
        "Enter a number to choose an option:\n",
        5};
    mmenu.options.push_back(" ");
    mmenu.options.push_back("Start a New Game");
    mmenu.options.push_back("Load a Previous Game");
    mmenu.options.push_back("Settings");
    mmenu.options.push_back("About");
    mmenu.options.push_back("Quit");

    do
    {
        mmenu.display();
        inp = pc.getInpn();
        switch(inp)
        {
        case 1: { char_menu(); if(!menu_return){
                cout << "\nDo you want to begin with the Tutorial? ( Y / N )\n";

                pc.inp = pc.getInps();

                if ( inpAffirm(pc.inp) ) { cout<<"\n\nLoading Tutorial Island..."; define_tutorial_island(); cout<<"Complete.\n\n"; pc.area=tut[1][2]; tutorial=true;}

                showarea();

                return;}
            } break;
        case 2: { load(); return; } break;
        case 3: settings(); break;
        case 4:
            {
                colSet(240, "\nTHE SILVER FLASK");
                cout << "\nCurrent version: 0.5.2\n";
                cout << "\n(c)2020 - 2025, Will Hooker\n\n\nA text-based adventure with all the usual bells and whistles.\n\n";
                cout << "This project began in September 2020 in a tent somewhere in Oregon.\n";
            }
            break;
        case 5:
            if (!game_settings.is_playing)
            {
                prln("Main Menu Skipped.");
                prln("Debug mode enabled."); admin = true;
                prln("CAUTION: some features may not function properly while in Debug Mode.");
                return;
            }
            break;
        case 6: cout<<"Obviously there are still some bugs.\n"; break;
        default: cout<<"Input error.\n"; break;
        }
    }while(inp!=mmenu.options_limit);
}

void startScreen()
{
    cout<<"\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

    cout<<"\tConquer the kingdom,\n\n\t...and discover...\n\n";

    cout<<"<================================>\n|>>>>>>[ THE SILVER FLASK ]<<<<<<|\n<================================>";

    cout<<"\n\t Version 0.5.2";

    cout<<"\n\n\n\n\n\n\n\n\n\n\n\n";
}

void begin()
{
    initializeGame("startup");
    //elist[3].showInfo();
    /*BUG --- GC --- elist ([3]?) corrupted here*/
    initializePlayer();

    initializeSettings();

    startScreen();

    main_menu();
}

void help()
{
	cout<<"\nWelcome to the Guide!\n";
	do
	{
	cout<<"\nWhat can I help you with?\n";
	cout<<" (1) Basic commands\n (2) Tutorial\n (3) Gameplay tips\n (4) Back\n";
	cin>>pc.inp;
	if(pc.inp=="1")
	{
	    string skill_commands[12] = {
	        "  'chop' - use a woodcutting axe to chop down a nearby tree",
	        "  'fire' - use a tinderbox and some wood to light a fire",
	        "  'mine' - use a pickaxe to mine veins of ore",
	        "  'smith' - use a hammer to forge arms and armor at any smithy",
	        "  'enchant' - use a gem to place a magical enchantment on weapons, armor, and accessories",
	        "  'sew' - use a needle, thread, and wool or leather to craft equipment",
	        "  'carve' - use a chisel to shape regular wood into useful items",
	        "  'forage' - search your surroundings for edible materials",
	        "  'hunt' - track down and kill nearby animals for food and hides",
	        "  'sow' - spread seeds over fertile farmland",
	        "  'harvest' - harvest any fully-grown crops",
	        "  'play' - use any instrument to play a song"};
	    cout<<"\nChoose a command list:\n";
		cout<<" (1) Basic commands\n (2) Skill commands\n (3) Back\n";
		int inp_n = check_int();

		switch(inp_n)
		{
		    case 1: {cout<<basic_command_list;} break;
		    case 2:
		        {
		            cout<<"\nSkill commands:\n";
		            for(int i=0;i<12;i++) {cout<<skill_commands[i]<<"\n";}
		        }
            default: break;
		}

	}
	else if(pc.inp=="2")
	{
	    cout<<"\nLeave this area to go to Tutorial Island?\n(you will be able to return at any time.)\n(1) Yes\n(2) No\n";
	    cin>>pc.inp;
	    if(pc.inp=="1") {cout<<"\n\nLoading Tutorial Island..."; define_tutorial_island(); cout<<"Complete.\n\n"; pc.area=tut[1][2]; showarea(); tutorial=true; return;}
	}
	else if(pc.inp=="3")
	{
		do
		{
		int tip=rand()%10+1;
		cout<<"\n";
		switch(tip)
		{
			case 1:
				cout<<"You slowly regain HP and MP as you move,\nlook around and perform other basic\nactions. You'll heal even faster with a campfire nearby.\n";
			break;
			case 2:
				cout<<"Your stats each impact different aspects of the game:\nStrength increases your damage in combat, \ndexterity increases your chance to flee or \ndodge attacks, intellect increases your magic \ndamage and experience gain, and \nluck increases your chances of finding gold or \ntreasure chests.\n";
			break;
			case 3:
				cout<<"You can perform multiple actions at a time if you know what you want to do\nnext; just type a space between your commands and and they will be executed in order.\n";
			break;
			case 4:
				cout<<"Any failed command in combat is a fumble that leaves you open to attack!\nSo make sure you know your skills & spells before you use them, or just stick\nto your basic attack...you can't misspell 'a'.\n";
			break;
			case 5:
				cout<<"Treasure chests and items on the ground will\nremain if you leave an area, but enemies will wander off.\n";
			break;
			case 6:
				cout<<"You can only use crafting or gathering skills\nif you have the right tools and supplies available;\nfor instance, you need an axe in your inventory and trees nearby to gather wood.\n";
			break;
			case 7:
				cout<<"A good first move is to check your inventory and 'equip' the cloak and wooden dagger\nyou'll find there. Even lowly Slimes can pose a threat when you're just starting out.\n";
			break;
			case 8:
				cout<<"Crafting skills have a multitude of perks that make them worth the relative effort;\nmany items and equipments are not sold in shops, especially those magical in nature.\nAdditionally, you can often sell crafted items for more gold\nthan their constituent parts.\n";
			break;
			case 9:
			    cout<<"Plenty of events, beneficial or detrimental, occur randomly and rarely;\nkeep an eye open as you travel for unique opportunities.\n";
			break;
			case 10:
                cout<<"There is no one path to success. Combat, crafting, farming, foraging, or questing -\nthese all increase your ability and will push you onward to victory.\n";
			break;
		}
		cout<<" (1) Next tip\n (2) Back\n";
		cin>>pc.inp;
		}while(pc.inp!="2");
	}
	}while(pc.inp!="4");
}

void read()
{
    if(pc.area.signs.size()>0)
    {
        cout<<"\nDid you want to read the sign nearby?\n";
        cin>>pc.inp;
        if(pc.inp=="yes"||pc.inp=="y"||pc.inp=="sign") {pc.area.readSign(); return;}
        else prln("Read an item from your inventory? (Y/N)");
        if( !inpAffirm( pc.getInps() ) ) return;
    }
    pc.showInv();;
    cout<<"\nRead item from which inventory slot?\n(Enter a number or '11' to return)\n";
    int num = check_int();
    if (num > 10 || num < 1) {cout << "You decide not to read anything.\n"; return;}
    if(pc.inv[num].type=="book")
    {
        if(pc.inv[num].mat=="skill")
        {
            int b=5;if(splist[68].unlock==true) b*=2;
            b+=pc.litlvl/2;
            switch(pc.inv[num].matb)
            {
                case 0: pc.wclvl+=b; break; //Woodcutting
                case 1: pc.fmlvl+=b; break; //Firemaking
                case 2: pc.mlvl+=b; break; //Mining
                case 3: pc.flvl+=b; break; //Smithing
                case 4: pc.cklvl+=b; break; //Cooking
                case 5: pc.clvl+=b; break; //Carving/woodworking
                case 6: pc.smlvl+=b; break; //Smithing
                case 7: pc.swlvl+=b; break; //Sewing
                case 8: pc.enchlvl+=b; break; //Enchanting
                case 9: pc.hlvl+=b; break; //Hunting
                default: pc.status="learned";
            }
            cout<<"\nYou leaf through the roughly bound pages, poring over old wisdom.\nYou've gained knowledge in "<<pc.inv[num].ench<<".\n";
            pc.litlvl++;
        }
        else if(pc.inv[num].mat=="spell")
        {
            splist[pc.inv[num].matb].unlock=true;
            cout<<"\nYou study the book and learn to use the "<<pc.inv[num].ench<<" ability.\nBattle command: 'cast "<<pc.inv[num].ench<<"'\n";
            pc.litlvl++;
        }
        cout<<"The book crumbles to dust.\n";
        pc.inv[num]=eq[0];
    }
    else if(pc.inv[num].name=="scroll")
    {
            noncomcast("null", pc.inv[num].matb);
            if(pc.inv[num].price>0)
            {
                cout<<"The scroll cracks and flakes from the energy of the spell.\n";
                pc.inv[num].price--;
            }
            else{cout<<"The scroll shudders violently and bursts into flame.\n"; pc.inv[num]=eq[0];}
    }
    else if(pc.inv[num].name=="recipe")
    {
        cout<<pc.inv[num].desc;
    }
    else if(pc.inv[num].name == "tome of light")
    {
        string words[3] ={
            "'When you're lost in the darkness, look for the light.'",
            "'Say not that you have found what you seek;\nSay rather, that which you did not know you sought, has found you.'",
            "'The needs of the Temple are no different from the needs\nof those around us. Help those in need, and you help us all.'"
        };
        prln("You read a stirring passage from the Tome of Light.\nIt reminds you of Brother Viri's words...");
        prln(words[rand()%3]);
    }
    else
        cout<<"You look the item over. You can't find any writing on it that you can understand.\n";
}

void mix()
{
    int item[2]; //Inventory slots of up to two items to mix
    int nonc; //Identifier for the non-container item being mixed
    equipment e_cont = eq[0];

    //Make sure the player has a container
    int container = searchinvtype("container"); //Inventory slot number of container (bottle, etc.)
    if ( !container ) { prln("You need a container to mix ingredients in!"); return; } else e_cont = pc.inv[container];

    pc.showInv();
    cout<<"Choose the first item:\n";
    item[0] = pc.getInpn();
    cout<<"Choose the second item:\n";
    item[1] = pc.getInpn();

    //create handles for container and non-container items
    //in order to tell the difference between them -
    //container's contents are removed and nonc is consumed
    if(container==item[0]) nonc=item[1]; else nonc=item[0];

    string contents=pc.inv[container].ench;

    //check that the item exists
    if(pc.inv[item[0]].name!="empty"&&pc.inv[item[1]].name!="empty")
    {
        //make sure player has a container
        if(container!=0)
        {
            cout << "\n"; //Arbitrary spacing
            //if player is mixing an item and a container
            if(container==item[0]||container==item[1])
            {
                if(contents=="water")
                {
                    if(pc.inv[nonc].type=="flower")
                    {
                        contents=pc.inv[nonc].mat+"-dye";
                        prln("You mix the flower into a vibrant "+pc.inv[nonc].mat+" dye.");;
                        pc.inv[container].ench=contents;
                        pc.inv[container].subt="dye";
                    }
                    pc.inv[nonc]=eq[0];
                }
                //if player is mixing colors (can only be performed with raw ingredient & dye)
                else if (pc.inv[container].subt=="dye")
                {
                    if(pc.inv[container].mat=="red-dye")
                    {
                        if(pc.inv[nonc].mat=="blue") pc.inv[container].mat="purple-dye";
                        if(pc.inv[nonc].mat=="yellow") pc.inv[container].mat="orange-dye";
                        if(pc.inv[nonc].mat=="red") pc.inv[container].mat="crimson-dye";
                        if(pc.inv[nonc].mat=="purple") pc.inv[container].mat="maroon-dye";
                    }
                    if(pc.inv[container].mat=="blue-dye")
                    {
                        if(pc.inv[nonc].mat=="blue") pc.inv[container].mat="cerulean-dye";
                        if(pc.inv[nonc].mat=="yellow") pc.inv[container].mat="green-dye";
                        if(pc.inv[nonc].mat=="red") pc.inv[container].mat="purple-dye";
                        if(pc.inv[nonc].mat=="purple") pc.inv[container].mat="indigo-dye";
                    }
                    if(pc.inv[container].mat=="yellow dye")
                    {
                        if(pc.inv[nonc].mat=="blue") pc.inv[container].ench="green-dye";
                        if(pc.inv[nonc].mat=="yellow") pc.inv[container].ench="sunny-dye";
                        if(pc.inv[nonc].mat=="red") pc.inv[container].ench="orange-dye";
                        if(pc.inv[nonc].mat=="purple") pc.inv[container].ench="brown-dye";
                    }
                    prln("You mix the two colors to make "+pc.inv[container].ench+".");
                    pc.inv[nonc]=eq[0];
                }
                else if (contents=="none")//if bottle is empty
                {
                    cout<<"You need water to mix the item into!\n";
                }
                else
                    cout<<"You can't mix those things together.\n";
            }
            else //if player is mixing two items into a container
            {
                if(contents=="water"){
                if(pc.inv[item[0]].name=="medicinal herb")
                {
                    if(pc.inv[item[1]].name=="snowflower")
                    {
                        cout<<"You mix the two items and create a mana potion.\n";
                        pc.mpot++;
                        pc.inv[item[0]]=eq[0];
                        pc.inv[item[1]]=eq[0];
                        pc.inv[container]=eq[0];
                    }
                    else if(pc.inv[item[1]].name=="torchweed")
                    {
                        cout<<"You mix the two items and create a health potion.\n";
                        pc.hpot++;
                        pc.inv[item[0]]=eq[0];
                        pc.inv[item[1]]=eq[0];
                        pc.inv[container]=eq[0];
                    }
                    else if(pc.inv[item[1]].name=="berry"&&pc.inv[item[1]].mat=="green")
                    {
                        cout<<"You mix the two items and create an antidote potion.\n";
                        pc.apot++;
                        pc.inv[item[0]]=eq[0];
                        pc.inv[item[1]]=eq[0];
                        pc.inv[container]=eq[0];
                    }
                }
                else if((pc.inv[item[0]].name=="lobelia"&&pc.inv[item[1]].name=="dandelion")||(pc.inv[item[1]].name=="lobelia"&&pc.inv[item[0]].name=="dandelion"))
                {
                    cout<<"\nYou mix the two items and create a potent poison.\n";
                    pc.inv[container].ench="poison";
                    pc.inv[item[0]]=eq[0];
                        pc.inv[item[1]]=eq[0];
                }
                else if((pc.inv[item[0]].name=="flour"&&pc.inv[item[1]].name=="dandelion")||(pc.inv[item[1]].name=="flour"&&pc.inv[item[0]].name=="dandelion"))
                {
                    cout<<"\nYou mix the two items and create a Luck Potion.\nYou throw in a coin for extra luck.\n";
                    pc.inv[container].ench="luck-potion";
                    pc.inv[container].subt="potion";
                    pc.inv[item[0]]=eq[0];
                    pc.inv[item[1]]=eq[0];
                    pc.gp--;
                }
                else if((pc.inv[item[0]].name=="snowflower"&&pc.inv[item[1]].name=="lobelia")||(pc.inv[item[1]].name=="snowflower"&&pc.inv[item[0]].name=="lobelia"))
                {
                    cout<<"You mix the two items and create an Intellect Potion.\n";
                    pc.inv[container].ench="intellect-potion";
                    pc.inv[container].subt="potion";
                    pc.inv[item[0]]=eq[0];
                        pc.inv[item[1]]=eq[0];
                }
                else if((pc.inv[item[0]].id==getItemID("molten", "core")&&pc.inv[item[1]].id==getItemID("red", "berry"))||(pc.inv[item[1]].id==getItemID("molten", "core")&&pc.inv[item[0]].id==getItemID("red", "berry")))
                {
                    cout<<"You mix the two items and create a Strength Potion.\n";
                    pc.inv[container].ench="strength-potion";
                    pc.inv[container].subt="potion";
                    pc.inv[item[0]]=eq[0];
                        pc.inv[item[1]]=eq[0];
                }
                else if((pc.inv[item[0]].id==getItemID("none", "feather")&&pc.inv[item[1]].id==getItemID("green", "berry"))||(pc.inv[item[1]].id==getItemID("none", "feather")&&pc.inv[item[0]].id==getItemID("green", "berry")))
                {
                    cout<<"You mix the two items and create a Dexterity Potion.\n";
                    pc.inv[container].ench="dexterity-potion";
                    pc.inv[container].subt="potion";
                    pc.inv[item[0]]=eq[0];
                    pc.inv[item[1]]=eq[0];
                }
                else
                    prln("You can't mix those items.");
            }//if container has water
            else prln("You need water to make a potion!");
            }//if player is using two items
        }
        else//if no container present
            cout << "You need a container to mix your items in.\n";
    }
    else //if inv slot was empty
        cout<<"You must choose two items to mix.\n";
}

void dye()
{
    int dye=searchinvtype("container");
    string color_o=pc.inv[dye].ench;
    string color;

    //retrieve the color of the dye
    const int length = color_o.length();
    int length_n=length-4;
    color="";
    for(int y=0;y<length_n; y++)
    {
        color=color+color_o.at(y);
    }

    int item=0;
    if (dye>0&&pc.inv[dye].subt=="dye")
    {
        pc.showInv();;
        cout<<"Dye which item?\n";
        item=check_int();
        if(pc.inv[item].name!="empty")
        {
            if(pc.inv[item].type=="weapon"||pc.inv[item].type=="armor"||pc.inv[item].type=="accessory")
            {
                pc.inv[item].mat=color+"-dyed-"+pc.inv[item].mat;
            }
            else
                cout<<"You can only dye equipment (weapons, armor, or accessories.)\n";
        }
    }
    else
        cout<<"You don't have any dye! Mix up flowers and water to make some!\n";
}

void fill()
{
	int container = searchinvtype("container");
    if ( !container ) {prln ("You don't have a container to fill!"); return;}

    //Cycle through containers
		for(int y=container;y<11;y++)
		{
			if(pc.inv[y].type=="container"&&pc.inv[y].ench=="none")
			{
			cout<<"Use the "<<pc.inv[y].name<<"? (y/n)\n";
			cin>>pc.inp;
			}
			if(pc.inp=="y")
			{
				if(pc.area.name == "Sacred Forest Grove" && pc.area.foe.name == "Forest Guardian" && !pc.area.foe.alive )
				{
					cout<<"\nYou lean down and fill the container with\nsparkling sap from the stump of the Guardian Tree.\n";
					pc.inv[y].ench="mystic-sap";
					pc.area.foe = null_enem;
				}
				else if(pc.area.name=="Pasture")
				{
				    if(!m_data.milk_ready) {prln("This cow isn't ready yet."); return;}
				    else {
                        cout << "\nYou set beside the cow and milk it into your container.\n";
                        pc.inv[y].ench = "milk";
                        m_data.milk_ready = false;
				    }
				}
				else if(pc.area.name=="Swamp")
				{
					cout<<"\nYou lean down and fill the container with fetid swampwater.\n";
					pc.inv[y].ench="swampwater";
				}
				else if( pc.area.name == "Sylvan Forest" && timen > 7)
				{
					cout<<"\nYou deftly catch a firefly.\n";
					pc.inv[y].ench="firefly";
				}
				else if(pc.area.water==true)
				{
					cout<<"\nYou fill your container with water.\n";
					pc.inv[y].ench="water";
				}
				else cout<<"\nYou don't see anything you could fill a container with.\n";
			return;
			}
		}
}

void poison()
{
    int psn=searchinv(288);
            int target;
            int area_x=pc.area.x;
            int area_y=pc.area.y;
            if(psn>0)
            {
                cout<<"\nDo you want to...\n(1) Poison a person\n(2) Poison a weapon\n(3) Return\n\n";
                cout<<"Enter a number:\n";
                cin>>pc.inp;
            }
            if(pc.inp=="3") return;
            else if(pc.inp=="1")
            {
                cout<<"Who do you want to poison?\n";
                for(int x=0;x<3;x++)
                {
                    if(pc.area.npcs[x].name!="null")
                    {
                        cout<<"-"<<pc.area.npcs[x].name<<"\n";
                    }
                }
                cin>>pc.inp;
                for(int x=0;x<3;x++)
                {
                    if(pc.inp==pc.area.npcs[x].name)
                    {
                        target=x;
                        break;
                    }
                }
                cout<<"\nYou stealthily poison your target; within seconds "<<pc.area.npcs[target].name<<" topples to the floor, dead.\n";
                if(pc.area.npcs[target].name=="Gareth")
                {
                    cout<<"  ->You've assassinated Knight Commander Gareth.\n    You should report your success to Valencia.\n";
                    loc[6][5].npcs[0].questcond=true;
                }
                pc.area.npcs[target]=nlist[0];
                loc[area_x][area_y].npcs[target]=nlist[0];
                pc.inv[psn] = eq[0];
            }//Poison -> person
            else if(pc.inp=="2")
            {
                pc.showInv();
                cout<<"\nWhat item did you want to poison?\n";
                int inp = check_int();

                //If item selected isn't a weapon or is already enchanted, return
                if(pc.inv[inp].type!="weapon"||pc.inv[inp].ench!="none") {cout<<"\nYou can't poison that!\n"; return;}
                else
                {
                    //Set weapon attributes; damage added is random 1 to 5
                    pc.inv[inp].ench = "poison";
                    pc.inv[inp].enchb = roll(5);

                    //Remove item
                    pc.inv[psn] = eq[0];

                    //Display
                    cout<<"\nYou open the vial of deadly poison and\nsmear a generous amount on your weapon.\n";
                }//Item selected is weapon
            }//Poison -> weapon
}

void shear()
{
	if ( !pc.shears ) {prln("You can't shear sheep with a sword!"); return; }
	if ( pc.area.name != "Pasture" ) {prln("You don't see any sheep to shear."); return; }
	if ( !pc.area.sheep ) {prln("These sheep are still in their birthday suits.\nCome back when they've grown some wool."); return;}

    cout<<"\nYou shear the sheep and gather some wool.\n";

    pickup( eq[ getItemID("none", "wool") ] );

    pc.area.sheep=false;
    loc[2][5].sheep=false;
}

void library()
{
    //pickup a random book, rather than selling a random book
    //to take advantage of the pickup function's book
    //assignment clause

    int choice;
    string menu_options="(1) Talk to librarian\n(2) Borrow book\n(3) Buy book\n(4) Leave\n";
    string lib_chat[3]={
    "'Hello, adventurer.'\n",
    "'Nice day for a read, isn't it?'\n",
    "'Try a good book, adventurer.'\n"};
    string error="'I'm not familiar with that phrase, I'm afraid.'\n";

    colSet(240, "Library\n");
    cout<<"\n\nA sudden change in atmosphere makes you blink momentarily, your eyes dazzled by drifting dust motes\nand warm light pouring in the windows.\nTwo staircases hug the circular, shelf-lined walls, leading up to a balcony.\nRows of shelves, interspersed with tables and chairs, fill the spacious central area.\nAt the center of the room is a circular desk cluttered with books and loose paper.\nBehind the desk, a weathered old elf peers at you through gold-rimmed pince nez.\n";

do{
        cout << menu_options;
        cin>>choice;
        while ( !(choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input; please re-enter.\n";cin >> choice;}
    switch(choice)
    {
    case 1: cout<<lib_chat[rand()%3];
        break;
    case 2://book exchange
        {
            cout<<"\nThe old elf shrugs. 'If you lend the library a book, I can lend you one in return.'\n(1) Yes\n(2) No\n";
            cin>>choice;
            if(choice==1)
            {
                for(int x=1;x<10;x++)
                {
                    if(pc.inv[x].name=="book")
                    {
                        cout<<"\nTrade in the book on "<<pc.inv[x].ench<<" for a random book from the library?\n";
                        cout<<"(1) Yes\n(2) No\n";
                        cin>>pc.inp;
                        if(pc.inp=="1")
                        {
                           string type[2]={"skill", "spell"};
                           int tnum=rand()%2;
                           pc.inv[x]=eq[0];
                           pickup(eq[getItemID(type[tnum], "book")]);
                        }//chose to exchange a book
                        else cout<<"\nYou decide not to exchange this book.\n";
                        break;
                    }//for x inventory slot, type 'book'
                }//end of for loop checking for books
            }//if choice==1
            else cout<<"\nYou decide not to exchange a book.\n";
        }//end of "if player chose to exchange a book." Damn.
        break;
    case 3://buy a book
        {
            string type[2]={"skill", "spell"};
                           int tnum=rand()%2;
            cout<<"\nThe old elf nods and sets a heavy tome on the desk, blowing some dust off the cover.\n";
            equipment temp=eq[getItemID(type[tnum], "book")];
            cout<<"\nIt appears to be a book about a certain "<<type[tnum]<<".\n";
            cout<<"(1) Buy (300g)\n(2) Leave\n";
            cin>>pc.inp;
            if(pc.gp>=300&&pc.inp=="1")
            {
                pickup(temp);
                pc.gp-=300;
            }
            else cout<<"He shrugs, putting the book back below the counter.\n";
        }
        break;
    case 4:
        {
            cout<<"\nThe librarian returns to his books, waving you away mildly.\n";
        }
        break;
    default:
            cout<<"\nHe blinks. 'I'm sorry, I don't understand that tongue.'\n";
        break;
    }//end of choice switch/case
    } while(choice!=4);
}

void inn()
{
    int mgt=rand()%2;
    struct minigame{
        string name;
        string desc;
    } mg[2];

    mg[0]={"Fortuneteller", "There is an old woman shuffling cards at a back table.\n"};
    mg[1]={"Liar's Dice", "There is a group of soldiers playing dice at a back table.\n"};

    if(game_settings.sound) mciSendString("play C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\inn.wav", NULL, 0, NULL);
    colSet(240, "Inn\n\n");
    cout<<"The heavy oak door creaks loudly as you enter, but the sound is lost in the din\nof the inn's common room. Travelers, townsfolk, soldiers and Arcanum alumni chatter\nand laugh loudly around a few battered, beer-stained tables.\n";
    cout<<"\nThere is a man sitting at the bar.\n";
    cout<<mg[mgt].desc;
    cout<<"The owner, a portly, balding man in his late thirties, is cleaning a glass with a small smile.\n";
    cout<<"He looks up as you approach.\n";

   do{
   cout<<"'Well met, stranger! What can I do for ya?'\n";
   cout<<"(1) Talk to innkeeper\n(2) Buy food & drink\n(3) Rent a room\n(4) Talk to patron\n(5) ";
   cout<<mg[mgt].name<<"\n(6) Leave\n";
   cin>>pc.inp;
   if(pc.inp=="1")
   {
       string goss[7]={
            "They say flowers grow even in the frozen lands to the north...\nAye, even in the wastes around a volcano, flowers'll grow.",
            "Not many elves through here, but those as'll talk tell\nof a city in the trees, in the southeast.",
            "There was another traveler that came through here, while back...\nTalked my ear off, he did, but knew the land better than most.",
            "Damn pixies ran off with my best wines again...",
            "Some stonemasons came through, not too long ago; said they'd\nhad enough trying to build that southern bridge, too dangerous, I guess.\nApparently their foreman is trying to finish it on his own...",
            "One'a them Northmen came through, from the village of Winterhold.\nHardy types, them, though they don't say much.",
            "Word is, the goblins up north have problems of their own, giant\ninsects pick them off in the night. All the better for us, I say."
       };
       int gosn=rand()%7;
       cout<<"\nThe innkeeper leans forward conspiratorially.\n\n";
       cout<<"'"<<goss[gosn]<<"\n\n";

       string s_end[3] = {
           "'Anyhow, that's what I hear.'",
           "'But what do I know? I just tend the inn.'",
           "'...that's old news, now, anyways.'"
       };

       cout << "He returns to wiping down the bar.\n" << s_end[rand()%3] << "\n\n";
   }
   else if (pc.inp=="4")
   {
       theNeedful();
   }
   else if(pc.inp=="2")
   {
       cout<<"Gold: "<<pc.gp<<"\n";
       cout<<"(1) buy ale (20gp)\n(2) buy cooke=d meat (10gp)\n";
       cin>>pc.inp;
       if(pc.inp=="1")
       {
           if(pc.gp<20){cout<<"'Sorry, friend, not without the coin.'\n";}
           else{equipment tempit=eq[64]; tempit.ench="ale"; pickup(tempit); pc.gp-=20;}
       }
       else if(pc.inp=="2")
       {
           if ( pc.gp < 10 ) { cout << "'Sorry, friend, not without the coin.'\n";}
           else { pickup( eq[ getItemID( "none", "cooked meat") ] ); pc.gp-=10;}
       }
       else (cout<<"'Sorry, I don't understand..'\n");
   }
   else if(pc.inp=="3")
   {
       cout<<"Rent a room for the night? (cost: " << m_data.inn_cost << ")\n";
       cout<<"(1) yes\n(2) no\n";
       cin>>pc.inp;
       if(pc.inp=="1")
       {
           cout<<"\nYou hand the innkeeper some coins and head to your room.\n\n";
           pc.gp-=m_data.inn_cost; pc.hp=pc.hpmax; pc.mp=pc.mpmax;
           day++; timen=0;
           dream();
           cout<<"After a good night's rest, you return to the inn common room.\n";
           if(pc.gp <= 0) prln("Your stay was so expensive you couldn't afford it!\nYou're in debt for " + to_string( abs(pc.gp) ) + " coins.\n");
       }
   }
   else if(pc.inp=="5")
   {
        if(mgt==0) dekara();
        else liars_dice();
   }
   else if(pc.inp=="6") cout<<"The innkeeper waves amiably as you leave. 'We hope to see you again sometime!'\n";
   else cout<<"'I'm sorry, I don't understand..'\n";
   } while(pc.inp!="6");
   if(game_settings.sound) mciSendString("stop C:\\Users\\vynew\\Desktop\\C++\\the_silver_flask\\resources\\game_settings.sound\\inn.wav", NULL, 0, NULL);
}

void mill()
{
    cout<<"\nYou enter the windmill.\n";
    do
    {
        cout<<"\nWhat will you do?\n";
        cout<<"(1) Grind flour\n(2) Leave\n";
        pc.inp = pc.getInps();
        if(pc.inp=="1")
        {
            int x=searchinv(85);
            if(x!=0)
            {
                cout<<"You grind some wheat, filling a burlap sack with flour.\n";
                pc.carrywt-=pc.inv[x].wt;
                pc.inv[x]=eq[86];
            }
            else cout<<"You don't have any wheat to grind.\n";
        }
    }while(pc.inp!="2");
}

void load_building_interior()
{
    string inp;
    int obj_handle;
    bool has_bed, has_oven, has_anvil, has_chair, has_obelisk;

    //create temporary building for ease of access; values returned at end
    building bldg=pc.area.loc_bldg;

    //set switches for interior furniture
    for(int x=0; x<bldg.objects.size();x++)
    {
        if(bldg.objects[x].name=="bed") { has_bed=true; }
        if(bldg.objects[x].name=="oven") { has_oven=true; }
        if(bldg.objects[x].name=="anvil") { has_anvil=true; }
        if(bldg.objects[x].name=="chair") { has_chair=true; }
        if(bldg.objects[x].name=="obelisk") { has_obelisk=true; }
    }

    //display interior...
    bldg.display_interior();

    //run an input loop with interior-specific commands
    do
    {
        cout<<"\nWhat will you do?\n";
        cin>>inp;

        //check first to see if the player is looking for an object
        for(int x=0; x<bldg.objects.size();x++)
        {
            if(inp==bldg.objects[x].name)
            {
                cout<<"You look at the "<<bldg.objects[x].name<<"\n";
                cout<<"It contains: \n";
                bldg.objects[x].display_contents();
            }
        }

        //exit command
        if(inp=="exit"||inp=="leave")
            cout<<"You exit the "<<bldg.name<<".\n";
        else if(inp=="inv"||pc.inp=="inventory")
        {
            pc.showInv();;
        }
        else if(inp=="item"||inp=="items"||inp=="object"||inp=="objects"||inp=="place"||inp=="remove"||inp=="build"||inp=="create")
        {
            int wd=searchinv(getItemID("none", "wood"));
            int ir=searchinv(getItemID("iron", "ore"));
            int st=searchinv(getItemID("stone", "block"));
            static_object* temp= new static_object;

            cout<<"(1) Create object\n(2) Remove object\n";
            cin>>inp;
            if(inp=="1")
            {
                cout<<"Create which object?\n(1) Chest\n(2) Table\n(3) Chair\n(4) Planter\n(5) Bed\n(6) Anvil\n(7) Oven\n(8) Obelisk\n(9) Back\n";
                cin>>inp;
                cout<<"\n";
                if(inp=="1"||inp=="chest")
                {
                    if(wd>0&&ir>0)
                    {
                        cout<<"You fashion a chest out of wood and iron. It now resides in the house.\n";
                        *temp=chest;
                    }
                    else cout<<"You need 1 piece of wood and 1 iron ore to build a chest.\n";
                }
                else if (inp=="2"||inp=="table")
                {
                    if(wd>0)
                    {
                        cout<<"You fashion a table out of wood. It now resides in the house.\n";
                        *temp=table;
                    }
                    else cout<<"You need 1 piece of wood to build a table.\n";
                }
                else if (inp=="3"||inp=="chair")
                {
                    if(wd>0)
                    {
                        cout<<"You fashion a chair out of wood. It now resides in the house.\n";
                        *temp=chair;
                    }
                    else cout<<"You need 1 piece of wood to build a chair.\n";
                }
                else if (inp=="4"||inp=="planter")
                {
                    if(st>0)
                    {
                        cout<<"You fashion a planter out of stone. It now resides in the house.\n";
                        *temp=planter;
                    }
                    else cout<<"You need 1 piece of stone to build a planter.\n";
                }
                else if (inp=="5"||inp=="bed")
                {
                    if(wd>0)
                    {
                        cout<<"You fashion a bed out of wood and wool. It now resides in the house.\n";
                        *temp=bed;
                    }
                    else cout<<"You need 1 piece of wood and 1 piece of wool to build a bed.\n";
                }
                else if (inp=="6"||inp=="anvil")
                {
                    if(wd>0&&ir>0)
                    {
                        cout<<"You fashion a heavy anvil out of iron and stone. It now resides in the house.\n";
                        *temp=anvil;
                    }
                    else cout<<"You need 1 piece of iron and 1 piece of stone to build an anvil.\n";
                }
                else if (inp=="7"||inp=="oven")
                {
                    if(wd>0&&st>0)
                    {
                        cout<<"You fashion an oven out of wood and stone. It now resides in the house.\n";
                        *temp=oven;
                    }
                    else cout<<"You need 1 piece of wood and 1 piece of stone to build an oven.\n";
                }
                else if (inp=="8"||inp=="obelisk")
                {
                    if(st>0)
                    {
                        cout<<"You fashion an obelisk out of stone. It now resides in the house.\n";
                        *temp=obelisk;
                    }
                    else cout<<"You need 1 piece of stone to build an obelisk.\n";
                }
                cout<<"Enter a one-word description (adjective) for your item:\n(enter 'none' to skip)\n";
                cin>>pc.inp;
                if(pc.inp!="none") temp->adj=pc.inp; else temp->adj="null";

                bldg.objects.push_back(*temp);

                delete temp;
            }
        }
        else if(inp=="look")
        {
            cout<<"Look at what?\n";
            cin>>inp;
            if(inp=="area"||inp=="around"||inp=="inside"||inp=="intererior")
            {
                bldg.display_interior();
            }//look at area

            //if player enters something not recognized as a look command,
            //this bit will cycle through the contents of the
            //building looking for a match.
            else
            {
                //cycle through objects to check against player input
                for(int x=0; x<bldg.objects.size();x++)
                {
                    if(inp==bldg.objects[x].name)
                    {
                        cout<<"It is a(n) ";

                        if(bldg.objects[x].adj!="null") cout<< bldg.objects[x].adj;
                        else cout<<"ordinary";
                        cout<<" "<<bldg.objects[x].name<<";\n"<<bldg.objects[x].desc<<"\n";


                        if(bldg.objects[x].contents.size()>0)
                        {
                            cout<<"It contains: \n";
                            bldg.objects[x].display_contents();
                        }
                        break;
                    }
                }//end of cycle objects
                //cycle through decorations to check against player input
                for(int x=0; x<bldg.deco.size();x++)
                {
                    if(inp==bldg.deco[x].name)
                    {
                        cout<<"It is an ordinary "<<bldg.deco[x].desc<<"\n"<<bldg.deco[x].name<<"\n";
                        break;
                    }
                }//end of cycle deco
            }
        }//look command
        else if(inp=="put"||inp=="stash"||inp=="store")
        {
            cout<<"Choose a nearby object to store items in.\n";
            for(int x=0;x<bldg.objects.size();x++)
            {
                cout<<"-"<<bldg.objects[x].name<<"\n";
            }
            cin>>inp;
            for(int x=0; x<bldg.objects.size();x++)
            {
                if(inp==bldg.objects[x].name)
                {
                        bldg.objects[x].display_contents();
                        obj_handle=x;
                        break;
                }
            }
            if(bldg.objects[obj_handle].contents.size()>=bldg.objects[obj_handle].cap)
            {
                cout<<"There's no more room in the container!\n";
            }
            else
            {
                    int num;
                    pc.showInv();;
                    cout<<"\nStore which item?\n";
                    num=check_int();
                if(pc.inv[num].name!="empty")
                {
                    bldg.objects[obj_handle].contents.push_back(pc.inv[num]);
                    cout<<"\nYou put the "<<pc.inv[num].name<<" into the "<<bldg.objects[obj_handle].name<<".\n";
                    pc.inv[num]=eq[0];
                }
                else
                    cout<<"There's nothing there!\n";
            }
        }//store item command
        //take an item out of any object present
        else if(inp=="open"||inp=="take")
        {
            cout<<"Access the contents of which item?\n";

            bldg.displayObjects();

            cin>>inp;
            for(int x=0; x<bldg.objects.size();x++)
            {
                if(inp==bldg.objects[x].name)
                {
                        bldg.objects[x].display_contents();
                        obj_handle=x;
                        break;
                }
            }
            cout<<"Take which item from the "<<bldg.objects[obj_handle].name<<"?\n(enter a number)\n";
            int inpn;
            inpn=check_int();
            if(inpn<bldg.objects[obj_handle].contents.size())
            {
                cout<<"You retrieve the "<<bldg.objects[obj_handle].contents[inpn].name<<" out of the "<<bldg.objects[obj_handle].name<<".\n";
            }
            else cout<<"There's nothing there!\n";
        }//open/take command
        else if(inp=="decorate")
        {
            string desc;
            decoration* temp=new decoration;
            cout<<"\nWhat item are you going to decorate the building with?\n";
            cout<<"(enter any one word. You don't need to have the item; it's just a decoration.)\n";
            cin>>temp->name;
            cout<<"\nWhat kind of object is it?\n(describe the item.)\n";
            getline(cin, desc);//this getline soaks up the newline character (?)
            getline(cin, temp->desc);
            cout<<"Where are you going to put the item?\n(e.g. 'wall', 'floor', 'outside')\n";
            cin>>temp->loc;
            cout<<"You put a decorative "<<temp->name<<" on the "<<temp->loc<<" of the "<<bldg.name<<"\n";
            bldg.deco.push_back(*temp);
            delete temp;
        }
        else if(inp=="sleep"||inp=="rest")
        {
            if(has_bed)
            {
                cout<<"You lay down in the bed and rest for the night.\n";
                pc.hp=pc.hpmax;
                pc.mp=pc.mpmax;
                timen=0;
                day++;

                dream();
            }
            else
            {
                rest();
            }
        }
        else if(inp=="cook") if(has_oven) cook();
        else if(inp=="bake") if(has_oven) bake();
        else if(inp=="smith") if(has_anvil) smith();
        else if(inp=="temper") if(has_anvil) temper();
        else cout<<"Command not recognized, please try again.\n";
    }while(inp!="exit"&&inp!="leave");

    //set the real building to match the temporary
    pc.area.loc_bldg=bldg;
}

void look()
{
	cout << "Look at what?\n";
	cin >> pc.inp;
	if( pc.inp == "at" ) cin >> pc.inp;

	for(int x=0;x<3;x++)//if player entered the name of an npc
	{
		if(strCase(pc.inp, "upper")==pc.area.npcs[x].name)
			{   cout<<"\nYou look at "<<pc.area.npcs[x].name<<"\n\n";
			    cout<<pc.area.npcs[x].appear;}
	}

	//If player enters the name of a side-quest character
	if(m_data.needful_nearby)
        if(pc.inp==needful.n||pc.inp==needful.adj||pc.inp==needful.mf)
            cout<<"This "<<needful.showName("full")<<" is looking for "<<aoran(eq[m_data.needful_item_id].mat, 0)<<eq[m_data.needful_item_id].showName()<<"\n";
    if(m_data.sick_nearby)
        if(pc.inp==sick.n||pc.inp==sick.adj||pc.inp==sick.mf)
            cout<<"This "<<lost.showName("full")<<" isn't looking so good;\n"<<sick.pers<<" is in dire need of a " <<m_data.sick_potion_needed << " potion.\n";
	if(m_data.lost_in_party)
        if(pc.inp==lost.n||pc.inp==lost.adj||pc.inp==lost.mf)
            cout<<"This "<<lost.showName("full")<<" has been traveling with you for some time.\n"<<lost.pers_up<<" is accompanying you to " << lost.pos << " destination, "<<loc[m_data.lostx][m_data.losty].name<<".\n";

    //If player looks at area loot by name
    if(pc.inp==pc.area.loot.name&&pc.area.loot.name!="empty")
    {
        cout<<"\nYou look at the "<<pc.area.loot.name<<" on the ground.\n";
        cout<<"\n"<<pc.area.loot.desc;
    }

    //If player looks at map features
    //Water, wood, mine, smithy, chest, fire, toolshop, potshop, armory, inn, etc
    if( (pc.inp == "water") && pc.area.water) prln("You look at the surface of the water.\nYou see your own reflection staring back at you.\n\nIs it a shining face filled with hope?\n...Or a gloomy face full of fear?");
    if( (pc.inp == "wood" || pc.inp == "trees" || pc.inp == "tree") && pc.area.wood) prln("You look up at a nearby tree.\nYou can see glimpses of sky through gaps between its leafy branches.\nA line of ants marches slowly up the trunk.");
    if( (pc.inp == "smithy" || pc.inp == "anvil") && pc.area.smithy) prln("You inspect the cold forge.\nThe scarred iron anvil has been here for some time.\nCrumbling ashes litter the furnace floor.");
    if( (pc.inp == "mine" || pc.inp == "ore" || pc.inp == "vein") && pc.area.mine) prln("You inspect the rock surface,\nfollowing the mineral patterns with a finger.\nYou see traces of several different ores.");
    if( (pc.inp == "fire" || pc.inp == "campfire") && pc.area.fire) prln("You stare into the fire.\nYou see all possible futures in the twisting tongues of flame.\n\n...You look away again, blinking to clear your eyes.");
    if( (pc.inp == "chest") && (pc.area.chest > 0) ) prln("You examine the old treasure chest. It's clearly been here awhile.\nThe wood has started to swell, splintering around the rusted iron bindings.\nIf there was ever a lock, it's long gone now.");
    if( (pc.inp == "fish" || pc.inp == "fishing") && pc.area.water) prln("You plunge your head into the water\nand peer at the plethora of fish swimming below.\n\nA cloud of minnows disperses around your face,\na dumbfounded river bass staring at you open-mouthed for a\nfew seconds before swimming away.");

    if(pc.inp=="ground"||pc.inp=="down")
    {
        cout<<"\nYou look at the ground.\n";
        if(pc.area.loot.name!="empty")
        {
            cout<<"\nThere is ";
            if (pc.area.loot.mat != "none") cout << aoran(pc.area.loot.mat, false);
            else cout << aoran(pc.area.loot.name, false);
            cout << pc.area.loot.showName() << " lying abandoned there.\n";
            return;
        }
        if( ( pc.area.foe.name!="" && pc.area.foe.name!="NULL" )&&!pc.area.foe.alive)
            {prln("There is the body of "+aoranf(pc.area.foe.name, false)+" lying here."); return;}

        cout<<"You see nothing but ";

        if(pc.area.name=="City Gate South"||pc.area.name=="City Gate West"||pc.area.name=="City Gate North"||pc.area.name=="City Gate East"||pc.area.name=="City Center")
        {
            cout<<"worn cobblestones";
        }
        else if(pc.area.name=="Village"||pc.area.name=="Grassland Path")
        {
            cout<<"the wide dirt road. Pretty clean for being dirt";
        }
        else if(pc.area.wood)
        {
            cout<<"grass, leaves and ferns";
        }
        else if(pc.area.name=="Grassland")
        {
            cout<<"waist-high grass. It rustles as you walk through";
        }
        else if(pc.area.name=="Tundra"||pc.area.name=="Tundra, Path's End"||pc.area.name=="Ice Fields")
        {
            cout<<"miles of pristine snow, broken occasionally by trees, rocks, or animal tracks";
        }
        else if(pc.area.name=="Lava Plain")
        {
            cout<<"smooth, ridged ground, formed of long-cooled\nlava flows from the nearby volcano.\nCracks in the glasslike surface expose veins of glowing magma";
        }
        else if(pc.area.name=="Swamp")
        {
            cout<<"brackish water and mud, and the occasional sickly-looking reed";
        }
        else if(pc.area.name=="Main Hall"||pc.area.name=="Throne Room")
        {
            cout<<"rich carpeting and polished stone floors";
        }
        else if(pc.area.name=="Ballroom")
        {
            cout<<"glossy marble floors";
        }
        else
        {
            cout<<"the ground. Big surprise there.";
        }
        cout<<".\n";
    }

    if(pc.inp=="north"||pc.inp=="n")
    {
        cout<<"\nShading your eyes, you look north.\n";
        if(loc[pc.area.x][pc.area.y-1].id!=0)cout<<"You see "<<loc[pc.area.x][pc.area.y-1].name<<".\n";
        else cout<<"The land that way is bordered by steep mountains.\n";
        if(!pc.area.north) cout<<"It looks like you can't get there from here, though.\n";
    }
    if(pc.inp=="south"||pc.inp=="s")
    {
        cout<<"\nShading your eyes, you look south.\n";
        if(loc[pc.area.x][pc.area.y+1].id!=0)cout<<"You see "<<loc[pc.area.x][pc.area.y+1].name<<".\n";
        else cout<<"The land that way is bordered by steep mountains.\n";
        if(!pc.area.south) cout<<"It looks like you can't get there from here, though.\n";
    }
    if(pc.inp=="east"||pc.inp=="e")
    {
        cout<<"\nShading your eyes, you look east.\n";
        if(loc[pc.area.x+1][pc.area.y].id!=0)cout<<"You see "<<loc[pc.area.x+1][pc.area.y].name<<".\n";
        else cout<<"The land that way is bordered by steep mountains.\n";
        if(!pc.area.east) cout<<"It looks like you can't get there from here, though.\n";
    }
    if(pc.inp=="west"||pc.inp=="w")
    {
        cout<<"\nShading your eyes, you look west.\n";
        if(loc[pc.area.x-1][pc.area.y].id!=0)cout<<"You see "<<loc[pc.area.x-1][pc.area.y].name<<".\n";
        else cout<<"The land that way is bordered by steep mountains.\n";
        if(!pc.area.west) cout<<"It looks like you can't get there from here, though.\n";
    }

    //Items in inventory by name only; displays first item that matches
    for(int x=0;x<11;x++)
    {
        if(pc.inp==pc.inv[x].name)
        {   cout<<"\nYou have a "<<pc.inp<<" in your pack. You take it out and look at it.\n\n";
            pc.inv[x].showData("");}
    }

	if(pc.inp=="self"||pc.inp==pc.name||pc.inp == "me")
    {
        chars();
    }
	else if(pc.inp=="area"||pc.inp=="around"||pc.inp=="location")
    {
		    prln("You examine your surroundings.");
		    showarea();
    }
    else if(pc.inp=="people"||pc.inp=="npcs"||pc.inp=="npc"||pc.inp=="person")
    {
        int n=0;
        cout<<"\n";
        for(int x=0;x<3;x++)
        {
            if(pc.area.npcs[x].id != 0) {pc.area.npcs[x].showIdle(); n++;}
        }
        if(pc.comp.name!="empty") pc.comp.idles();
        if(n==0) cout<<"There isn't anyone else here.\n";
    }
    else if(pc.inp==pc.area.loc_bldg.name||pc.inp=="building"||pc.inp=="structure")
        cout<<pc.area.loc_bldg.ext_desc;

    else if(pc.inp=="sign") pc.area.readSign();

    else if(pc.inp=="map")
    {
        cout<<"\nYou look at your map of Samlund.\n";
        cout<<"\nIt shows a small, green countryside bordered by steep mountains.\n";
        cout<<"\nFed by meltwaters in the north, a river winds through the valley.\n";
        cout<<"\nIt looks incomplete. You'll have to fill it in as you go.\n";
    }
	else if(pc.inp=="inv"||pc.inp=="inventory"||pc.inp=="item"||pc.inp=="items")
	{
		pc.showInv();
		cout<<"\nEnter the slot number of the item you want to inspect, or 11 to return: \n";

		int num;
		num = pc.getInpn();
		if(num>=11) return;

		cout<<"\n"<<pc.inv[num].desc;
		cout<<"It feels like it weighs "<<pc.inv[num].wt<<" pounds.\n";
		cout<<"It's probably worth about "<<pc.inv[num].price<<" gold.\n";

		if(pc.inv[num].type=="container")
        {
            if(pc.inv[num].subt=="pet")
            {
                cout<<"\nIt contains a live "<<pc.inv[num].ench<<".\n";
            }
            else if(pc.inv[num].ench!="none")
            {
                cout<<"\nIt's full of "<<pc.inv[num].ench<<".\n";
            }
        }
        else
        {
            if(pc.inv[num].ench!="none")
            {
                cout<<"There is a faint aura of arcane "<<pc.inv[num].ench<<" around it.\n";
            }
        }
	}
	else if(pc.inp=="weapon")
		prln(pc.weap.desc);
	else if(pc.inp=="armor")
		prln(pc.arm.desc);
	else if(pc.inp=="accessory")
		prln(pc.acc.desc);

	else if(pc.inp=="enemy"||strCase(pc.inp, "upper")==pc.area.foe.name||pc.inp == pc.area.foe.name)
	{
		if ( pc.area.foe.name != "null" )//if(pc.area.foe.alive == true)
		{
            {if (pc.area.foe.agg_lvl != "peaceful" && pc.area.foe.agg_lvl != "aggressive") pc.area.foe.agg_lvl = "peaceful";}
		    cout << "\nYou inspect the " << pc.area.foe.name<<".\n";
		    cout << "\nIt appears " << pc.area.foe.agg_lvl;
                if (pc.area.foe.agg_lvl == "aggressive") cout << " and may attack at any moment.\n";
                else cout << ", for the moment.\n";
		    cout << "\nIt seems ";

		    bool has_trait = false;
		    if (pc.area.foe.lvl > 6){
                if (pc.area.foe.str >= pc.area.foe.lvl/3) {cout << "exceptionally strong, "; has_trait = true;}
                if (pc.area.foe.intl >= pc.area.foe.lvl/3) {cout << "deceptively cunning, "; has_trait = true;}
                if (pc.area.foe.lck >= pc.area.foe.lvl/3) {cout << "whimsically fortunate, "; has_trait = true;}
		    }
		    if(!has_trait) cout << "pretty average ";

			cout << "\nand is "<<pc.area.foe.idle<<" a short distance away.\n";
			cout<<"\nIt has "<<pc.area.foe.hp<<" HP, "<<pc.area.foe.atk<<" attack, "<<pc.area.foe.def<<" defense,\nand looks ";
			if(pc.area.foe.dex<pc.dex)
				cout<<"pretty slow.\n";
			else
				cout<<"quite agile.\n";
		}
		else if(pc.area.foe.alive == false &&(pc.area.foe.id>0||pc.area.foe.name=="Slime"))
        {
            cout<<"There is the body of a "<<pc.area.foe.name<<" here.\n";
        }
		else
			cout<<"You look for an enemy, but you are alone.\n";
	}

	else if(pc.inp=="companion"||pc.inp==pc.comp.name)
		if(pc.comp.name!=" ") {pc.comp.showAppearance();}
		else if(m_data.lost_in_party) {lost.showAppearance();}
		else cout<<"You're traveling alone.\n";

    else if(pc.inp=="animal"||strCase(pc.inp, "upper") == pc.area.loc_fauna.name||pc.inp=="fauna")
    {
        if(pc.area.loc_fauna.name=="Fairy"&&pc.area.loc_flora.name=="toadstool")
        {
            cout<<"\nOn closer inspection, there are dozens of fairies nearby...where did they all come from?\n";
        }
        if(pc.area.loc_fauna.name!="null")
        {
            string cond[3] = {"young and weak", "mature and strong", "old and cunning"};
            cout<<"\nIt seems to be an ordinary "<<pc.area.loc_fauna.name<<".\n";
            cout<<"\nIt looks ";
            switch(pc.area.loc_fauna.lvl)
            {
                case 1 ... 3: cout<<cond[0]; break;
                case 4 ... 7: cout<<cond[1]; break;
                case 8 ... 10: cout<<cond[2]; break;
            }
            cout<<".\n";
        }
        else
            cout<<"There aren't any animals nearby; maybe you could try hunting for one?\n";
    }
    else if(pc.inp=="plant"||pc.inp==pc.area.loc_flora.name||pc.inp=="flora"||(pc.inp==pc.area.loc_flora.name&&pc.area.loc_flora.name!="null"))
    {
        if(pc.area.loc_fauna.name=="Fairy"&&pc.area.loc_flora.name=="toadstool")
        {
            cout<<"On closer inspection, the toadstool has tiny doors and windows...how odd...\n";
        }
        if(pc.area.loc_flora.name!="null")
            cout<<"It seems to be an ordinary "<<pc.area.loc_flora.name<<".\n";
        else
            cout<<"There aren't any plants nearby.\n";

            if(pc.area.wood) cout<<"There are some trees growing nearby.\n";
    }
    else if(pc.inp=="map")
        show_map();
	else if(pc.inp=="sky"||pc.inp=="sun"||pc.inp=="clouds"||pc.inp=="stars"||pc.inp=="moon"||pc.inp=="up"||pc.inp=="weather")
	{
	    string desc;
	    int col[10]={14, 14, 3, 3, 3, 13, 13, 8, 8, 8};

	    if(!pc.area.is_outside)
        {
            cout<<"You look up. You see nothing but the ceiling.\n";
            return;
        }

	    cout<<"\nYou look up at the sky, shielding your eyes with your hand.\n\n";
		switch(timen)
		{

			case 1 ... 2:
				desc="An orange sun is rising regally over a misty\nblue horizon. The clouds glow golden underneath,\nwhile still a sleepy lavender above. Morning birds join\na trilling chorus as the sky turns from\ndeep purple, to gold, to cerulean blue.\n";
			break;
			case 3 ... 5:
				desc="The sun has just crossed its zenith, but has\nnot yet begun to fall. The few clouds trailing\nlazily across the clear blue sky are wispy and white.\nThe earth is warm and the sun,\nright overhead, casts no shadow.\n";
			break;
			case 6 ... 7:
				desc="Sleepily sinking into the shadowed west,\nthe sun retires its rays, warm colors slowly\nfading from the sky. Crickets begin their rythmic\nchirp as a few early stars glitter overhead.\n";
			break;
			case 8 ... 10:
				desc="Stars glimmer overhead in the clear night sky,\nsplayed like so many jewels about a huge, pale moon.\nAn owl hoots pensively from the darkness,\nand far off in the distance, a wolf howls mournfully.\n";
			break;
		}
		colSet(col[timen], desc);
		colSet(curs.weath.color, curs.weath.desc);
		cout<<"\n\n";
	}

	//looking at something doesn't take any time.
	timen--;
}

void time_and_day()
{
    //set day and time
		timen++;
		if(timen>10)timen=1;

		//At the start of each new day
		if(timen==1)
        {
            day++;
            //Inn price decided
            m_data.inn_cost = ( (pc.lvl*10) + roll( pc.lvl*5 ) ) - ( (pc.karma/8) + pc.lck );
            //Sheep replenish
            m_data.wool_ready = true;
            //Cows...'replenish'
            m_data.milk_ready = true;
        }

		cout<<"\nDay "<<day<<": ";

		//Check if the season is changing
        //Print the name of the season in a seasonal color
        if(updateSeason(false))
        {
            string name_stylized;
            if (curs.name == "Spring") {year++; cout << "\nYEAR " << year << ":\n"; name_stylized = "^ S P R I N G ^";}
            if (curs.name == "Summer") name_stylized = "*__S U M M E R _";
            if (curs.name == "Fall") name_stylized = "~ F A L L ~";
            if (curs.name == "Winter") name_stylized = "_*_ W I N T E R _*_";

            cout << "\n\n\n\t";
            colSet(curs.color, name_stylized);
            cout << "\n\n\n";
        }
        else colSet(curs.color, curs.name);

        string tod;
        string todd;
        string prTod;
        int cn;

        cout<<"\n  ";

        switch(timen)
		{
			case 1 ... 2:{
				tod="Morning";
                todd="A cold, bright sun peers over the horizon."; cn=14;}
			break;
			case 3 ... 5:{
				tod="Afternoon";
				todd="The sun is high overhead, beginning to drift across its zenith.";cn=3;}
			break;
			case 6 ... 7:{
				tod="Evening";
				todd="Shadows lengthen as the sun begins to drift lower in the sky.";cn=13;}
			break;
			case 8 ... 10:{
				tod="Night";
				todd="The only light overhead is the soft radiance of the moon.";cn=8;}
			break;
		}

		//Set global time string variable
		times = tod;

		switch ( timen )
		{
		    case 1: times = "Early Morning"; break;
		    case 2: times = "Late Morning"; break;
		    case 3: times = "Early Afternoon"; break;
		    case 4: times = "Mid-Afternoon"; break;
		    case 5: times = "Late Afternoon"; break;
		    case 6: times = "Early Evening"; break;
		    case 7: times = "Late Evening"; break;
		    case 8: times = "Dusk"; break;
		    case 9: times = "Midnight"; break;
		    case 10: times = "Dawn"; break;
		}

		//Print out the time of day (morning, afternoon, etc.) in a specific color
		//and include a brief description
		prTod="("+tod+")";
		colSet(cn, prTod); cout<<"  "+todd+"\n";
}

void heal_step()
{
    //Effects of lingering toxins
    if(pctox > 0) {
        //Heal status over time
        pctox--;
        //Damage from poison
        if (pc.status == "poisoned") {
            int psndmg = roll(pc.hp/20);
            cout << "\nYou take " << psndmg << " damage from poison.\n";
            //Warning message at 1/3 health
            if (pc.hp < pc.hp/3) { cout << "You begin to feel gravely ill.\n";}
        }
    }

    if(pctox==0&&pc.status=="poisoned")
    {
        cout<<"\nThe poison in your body dissipates.\nYou begin to feel better.\n";
        pc.status = "OK";
    }

    //heal from alcohol
    if(pc.status=="drunk"&&pctox==6)
    {
        cout<<"You are mildly intoxicated.\n";
        pc.status="mildly intoxicated";
    }
    else if(pc.status=="mildly intoxicated"&&pctox==3)
    {
        cout<<"You are hungover.\n";
        pc.status="hungover";
    }
    else if(pc.status=="hungover"&&pctox==0)
    {
        cout<<"You've sobered up.\n";
        pc.status="OK";
        pc.intl++;
        pc.str--;
    }

    if(pc.hp<pc.hpmax)
		{
			if(pc.area.fire) pc.hp+=2;
			else pc.hp++;
		}
    if(pc.mp<pc.mpmax)
		{
			if(pc.area.fire) pc.mp+=2;
			else pc.mp++;
		}

    //heal companion
    if(pc.comp.name!="empty") pc.comp.healStep(pc.area.fire);
}

void hunger_and_thirst()
{
    int hunger = 1;
    int thirst = 2;

    //Increase depletion rate on higher difficulties
    if (game_settings.diff_desc == "Extreme") {hunger = 5; thirst = 5;}
    else if (game_settings.diff_lvl > 5) { hunger += game_settings.diff_lvl / 3; thirst = game_settings.diff_lvl / 3; }

    if ( pc.race == "Elven" ) {hunger = 0; thirst = 0;}
    pc.hunger -= hunger;
    pc.thirst -= thirst;

    switch (pc.hunger){
        case 80: { prln("You are feeling kind of hungry."); } break;
        case 60: { prln ("You are feeling very hungry."); } break;
        case 40: { prln("Your stomach growls. You are feeling very hungry."); } break;
        case 20: { prln ("You are starving. You need to find something to eat."); } break;
        case 10: { prln("You are beginning to feel weak from hunger.\nThere must be some food somewhere nearby..."); } break;
        case 0: { prln("Your body cannot sustain itself without energy any longer.\nYou fall to the ground, your vision fading to black.\n");
            pc.hp = 0; } break;
    }

    switch (pc.thirst){
        case 80: { prln("You are feeling kind of thirsty."); } break;
        case 60: { prln ("You are feeling very thirsty."); } break;
        case 40: { prln("Your throat is parched. You are feeling very thirsty."); } break;
        case 20: { prln ("You are becoming dehydrated. You need to find something to drink."); } break;
        case 10: { prln("You are beginning to feel weak from thirst.\nThere must be some water somewhere nearby..."); } break;
        case 0: { prln("Your body cannot sustain itself without water any longer.\nYou fall to the ground, your vision fading to black.\n");
            pc.hp = 0; } break;
    }
}

void update_step()
{
    //Console/format
    game_settings.console_title = "The Silver Flask: " + pc.name + " (" + to_string(pc.area.x) + ", " + to_string(pc.area.y) + " - " + pc.area.name + ")";
    setConCap(game_settings.console_title);

    lvlup(); //level up on begin step
    if(pc.comp.name!="empty"&&pc.comp.xp>=pc.comp.xpnxt) pc.comp.lvlup(); //level up companion
    heal_step();//heal player and companion
    if (game_settings.survival) hunger_and_thirst();
    setKarma(pc.karma, 0);//Update karma label and description, change 0 to increase

    //Quest Updates
    //The Sound of Music
    if(pc.clvl >= 5 && loc[7][10].npcs[0].q.lvl == 1 && loc[7][10].npcs[0].questcond == false)
    {
        gameMsg("Your carving level is now high enough to fix Aria's flute.");
        loc[7][10].npcs[0].questcond = true;
    }
    //Cleansing Light
    if(loc[6][7].npcs[0].q.lvl>=1&&loc[6][7].npcs[0].questcond==false)
    {
        if(pc.karma>=30)
        {
            loc[6][7].npcs[0].questcond=true;
            cout<<"\n  ->You feel a warmth suffuse your soul; it seems you've done much good in the land.\n   Perhaps Brother Viri has further guidance for you.\n";
        }
    }

    //Chance of enemy appearance
	if( rollfor(pc.area.echance, 100) && !tutorial )
    {
        pc.area.foe = createEnemy();

        prln(aoranf(pc.area.foe.name, true) + " moves into view.");

        if(pc.gp > 100 && pc.area.foe.name == "Bandit") pc.area.foe.agg_lvl = "aggressive";
    }

    //Chance of enemy attack
    if ( pc.area.foe.alive && !pc.area.foe.agg_lock && pc.area.foe.setAggression(pc.lvl, pctOf(pc.hp, pc.hpmax), pc.area.foe.checkStatsAreHigher(true, pc.str, pc.dex, pc.intl, pc.lck) ) ) { combat(); }
	//Advance time
	//Controls regrown/reset items
    time_and_day();
}

void addMatToBldg()
{
    int material_used;
    int wd=searchinv(getItemID("none", "wood"));
    int ir=searchinv(getItemID("iron", "ore"));
    int st=searchinv(getItemID("stone", "block"));

    if(pc.area.loc_bldg.name!="null")
    {
        material_used=pc.area.loc_bldg.build(wd, ir, st);
        switch(material_used)
        {
            case 1: material_used=wd; break;
            case 2: material_used=ir; break;
            case 3: material_used=st; break;
        }
        pc.inv[material_used]=eq[0];
    }
    else
    {
        int inp;
        cout<<"There's an open plot of land here;\nWhat do you want to build?\n";
        cout<<"(1) House\n(2) Back";
        inp=check_int();
        switch(inp)
        {
            case 1:{cout<<"You rope off an area and begin the foundation for a house.\n"; pc.area.loc_bldg=house;} break;
            case 2: {cout<<"You change your mind.\n";} break;
            default:{cout<<"That's not a building you can create!\n";} break;
        }
    }
}

player createNewPlayer(int nweap, int narm, int nacc)
{
    player new_player;
    new_player.createNew( "Jyce", "male", "Raszyra", "muscular", "golden", "crimson", "dusky", "flame", "Spellsword", 180, 180, 39, 100, 999,
                         1, 1, 4, 3, 2, 10, getItemID("woolen", "cloak"), getItemID("mithril", "wakizashi"), 11);
    return new_player;
}

void trophyCheck(string trophy, int chance) { if(!trophy_list[trophyGetID(trophy)].unlock&&rollfor(chance,100)) trophyGet(trophy, 0); }

void addDebug()
{
    string debug_add;

    prln("What do you want to add to the debug log?");
    cin.clear();
    getline(cin, debug_add);

    prln("\""+debug_add+"\"");

    debug_log.open("debug_log.txt");
    if(!debug_log.is_open()) prln("File error: could not open log");

    else {debug_log << debug_add << "\n";}

    debug_log.close();
    if(debug_log.is_open()) prln("File error: could not close log");
}

void readDebug()
{
    string debug_text = "null";

    debug_log.open("debug_log.txt");
    if(!debug_log.is_open()) {prln("File error: could not open file"); return;}
    else {
        prln("Debug Log\n");
        while (debug_log >> debug_text) {cout << debug_text << "\n";}
        if(debug_log.eof()) {prln("End of Debug Log");}

    debug_log.close();
    if(debug_log.is_open()) prln("File error: could not close file"); }//end else
}

bool is_custom_command=0;
int custom_command_timer=0;
int custom_command_id=0;
vector<string> custom_commands;

bool disable_error_flag = 0;
string emotes_list[30] = {"smile", "laugh", "frown", "cry", "dance", "pose", "jump", "crouch", "scowl", "glare", "snap", "clap", "think", "grin", "skip", "think", "sing"};

int main()
{

    cout <<"THE SILVER FLASK\n\n";

    cout <<"...is loading.\n";

    game_settings.sound=false;
    if(game_settings.sound) mciSendString("play ..\\resources\\game_settings.sound\\firehit.wav from 0 to 1000", NULL, 0, NULL);

	//Initialize game
	begin();

	int num;

    srand(time(NULL));


    //MAIN GAME LOOP
    //Check status, get and execute input
	do
	{
	    cout << "\n~+--------------------+~";
	    //date, time, season, heals, regrowth
	    update_step();

        //Roll for and display weather
        string prW;
        if(rollfor(1,5))curs.weath=curs.wopt[rand()%3];
		prW="  ("+curs.weath.name+") ";
		colSet(curs.weath.color, prW); cout<<curs.weath.desc<<"\n\n";

		//check for death and restart or quit
		if(pc.hp<=0) {Death();}

		//if alive, get input
		if(pc.hp>0)
		{
        game_settings.is_playing = true;

        cout<<"What will you do?\n";
		if(!is_custom_command) pc.inp = pc.getInps();
		cout << "~+--------------------+~\n";

        for (int i = 0; i < pc.custom_commands2.size(); i++){
            if ( pc.inp == pc.custom_commands2[i].key ) {
                pc.inp = pc.custom_commands2[i].command;
                break;
            }
        }

		//check for custom commands, may overlap with pre-coded commands
       for(int ct=0; ct<pc.custom_commands.size();ct++)
        {
            if(pc.inp==pc.custom_commands[ct].key)
            {
                is_custom_command=true;
                custom_command_timer=3;
                custom_command_id=ct;

                break;
            }
        }
        //execute custom commands in order via a counter
        if(is_custom_command)
        {
            int n=custom_command_id;
            int t=custom_command_timer-1;
            string comm=pc.custom_commands[n].c[t];

            pc.inp=comm;

            custom_command_timer--;

            if(custom_command_timer==0)
                {is_custom_command=false;}
        }

        //Spell command catch
        for (size_t i = 0; i < size(splist); ++i){
            if( pc.inp == splist[i].name ) {splist[i].showInfo(); disable_error_flag = 1;}
        }

        /*==========================
        [-----INPUT / COMMANDS-----]
        ===========================*/

        /*------SYSTEM FUNCTIONS-------*/
        /*
            Contents:

            -Quit
            -Main Menu
            -Exit Tutorial
            -Save
            -Load
            -Settings
            -In-game time and day
            -Enter a custom command set
            -Show custom command sets
            -miscellaneous tools
        */
		if (pc.inp=="quit")
			{
			    cout<<"See you later!\n";
                game_settings.is_playing = false;
			}
        if(pc.inp=="menu"||pc.inp=="main")
           main_menu();
        else if(pc.inp=="settings") {settings();}
        else if (pc.inp == "diff" || pc.inp == "difficulty") prln("Current difficulty level is '" + game_settings.diff_desc + "'.\n(You can change this from the 'settings' menu.)");
        else if ( pc.inp == "time" )
        {
            timen--;//this action takes no time
            cout << "\nYou squint up at the sky, gauging the time...\n";
            cout << "\nIt is " << times << ".\n";
        }
        else if(pc.inp=="exit")
        {
            tutorialExit();
            if(!tutorial) showarea();
        }
        else if(pc.inp=="help")
            help();
        else if (pc.inp=="commands"||pc.inp=="c")
            cout<<basic_command_list;
        else if(pc.inp == "check_admin" || pc.inp == "isadmin") if(admin)prln("Admin status: OK"); else prln("Admin status: NO");

        /*===================
        [----SAVE / LOAD----]
        ===================*/

        else if(pc.inp=="save" || pc.inp == "qsave")
        {
            //writeState();
            save();
        }
        else if(pc.inp=="load")
        {
            //loadState();
            //gameSetState();
            load();
        }


        /*------BUILDINGS-------*/

        else if(pc.inp=="shop")
        {
            if(pc.area.loc_shop.name!="null")
            {
                //Custom shop creation
                if(pc.area.loc_shop.name == "Wandering Archivist"){
                    Archivist("run");
                }
                //Basic shop creation
                else create_shop(pc.area.loc_shop.id, pc.area.loc_shop.name);
            }
        }
        else if(pc.inp=="enter"||pc.inp=="building"||pc.inp==pc.area.loc_bldg.name)
        {
            if(pc.area.loc_bldg.name!="null"&&pc.area.loc_bldg.complete)
            {
                load_building_interior();
            }
            else cout<<"You can't do that now!\n";
        }
        else if(pc.inp=="build"||pc.inp=="construct")
        {
            addMatToBldg();
        }


        /*====================
        [----QUICK CHEATS----]
        ====================*/
        /*Special commands that don't require Admin Mode.
            Mostly for debugging.

        */
        else if(pc.inp=="godmode")
        {
            pc.hp=10000;
            pc.mp=10000;
            pc.gp=10000;
            pc.str=10;
            pc.dex=10;
            pc.intl=10;
            pc.lck=10;
            admin=1;
            pc.inv[3]=eq[16];
            cout<<"You shrug off your mortal bounds and interact with the world as a god.\n";
        }
        else if (pc.inp == "setc") {
            pc.getCustomCommand2();
        }
        else if (pc.inp == "showc") pc.showCustomCommands2();
        else if (pc.inp == "statusbar") user_status_bar();
        else if (pc.inp == "showcolors") show_color_map();
        else if (pc.inp == "colortest")
        {
            colSet(COL_BLOCK_BLUE, "This is a test.\n");
            colSet(COL_BLOCK_TEAL, "This is a test.\n");
            colSet(COL_BLOCK_YELLOW, "This is a test.\n");
            colSet(COL_BLOCK_RED, "This is a test.\n");
            colSet(COL_BLOCK_GREEN, "This is a test.\n");
            colSet(COL_BLOCK_ORANGE, "This is a test.\n");
            colSet(COL_BLOCK_PURPLE, "This is a test.\n");
            colSet(COL_BLOCK_LIME, "This is a test.\n");
            colSet(COL_BLOCK_WHITE, "This is a test.\n");
            colSet(COL_BLOCK_GREY, "This is a test.\n");
            colSet(COL_BLUE, "This is a test.\n");
            colSet(COL_CYAN, "This is a test.\n");
            colSet(COL_GREEN, "This is a test.\n");
            colSet(COL_GREY, "This is a test.\n");
            colSet(COL_ORANGE, "This is a test.\n");
            colSet(COL_PURPLE, "This is a test.\n");
            colSet(COL_RED, "This is a test.\n");
            colSet(COL_WHITE, "This is a test.\n");
            colSet(COL_YELLOW, "This is a test.\n");
        }
        else if (pc.inp == "ymh") {

            pickup ( eq[getItemID("iron", "ore")] );
            pickup ( eq[getItemID("none", "coal")] );
            pickup ( eq[getItemID("silver", "ore")] );
            pickup ( eq[getItemID("golden", "ore")] );
            pickup ( eq[getItemID("none", "crystal")] );
        }
        else if (pc.inp == "unlockall") for (int i = 0; i < 68; i++) {splist[i].unlock = true; cout << "Unlocked " << splist[i].name << "\n";}
        else if (pc.inp == "archivist") if (pc.area.loc_shop.name != "Wandering Archivist") Archivist("create_and_run"); else Archivist("run");
        else if (pc.inp == "proc_quest") {
            npc temp = cast_npc[3];

            temp.dopt[0]="(1) Do you need help with anything?\n(2) Don't I know you from somewhere?\n(3) Leave\n";
            temp.info[0]="He thinks about it. 'Hmm, no, I don't think so -\nI have a lot of friends, but I don't go out often.\nI usually just stay in my space.'\n";
            temp.qline[1]="'Yes, please! I'm looking for an item - here, I'll write a description\nin your quest journal.'\n";
            //dialog options second - chat response second - quest response second
            temp.dopt[1]="(1) Here's the item you needed.\n(2) What else has been going on lately?\n(3) Leave\n";
            temp.info[1]="'Oh, nothing. There's not been a whole lot for me to do for the last few years.'\n";
            temp.qline[2]="'Well done!'\n";

            temp.dopt[2]="(1) How's that item working out?\n(2) What else has been going on lately?\n(3) Leave\n";
            temp.info[2]="'Oh, nothing. There's not been a whole lot for me to do for the last few years.'\n";

            temp.create("new");
            pc.area.npcs[0] = temp;
        }
        else if (pc.inp == "tower") wizardTower();
        else if (pc.inp == "path") mountainPath();
        else if(pc.inp == "create_enemy")
        {
            pc.area.foe = createEnemy();
            prln(aoran(pc.area.foe.name, true) + pc.area.foe.name + "moves into view.");
        }
        else if (pc.inp == "idk") prln("You shrug your shoulders nonchalantly.");
        else if (pc.inp == "saveitem")
        {
            pc.showInv();
            cout << "Save which item to file?\n";
            int num = pc.getInpn();

            pc.inv[num].saveToFile( pc.name, "_inv" + to_string(num) + "_", pc.inv[num].id);
        }
        else if (pc.inp == "loaditem")
        {
            cout << "Load item to which inventory slot?\n";
            int slot = pc.getInpn();
            cout << "Enter item id:\n";
            int id = pc.getInpn();

            pc.inv[slot].loadFromFile(pc.name, "_inv1_", 2);
        }
        else if(pc.inp == "saveinv")
        {
            pc.saveInventory();
        }
        else if(pc.inp == "loadinv")
        {
            pc.loadInventory();
        }
        else if (pc.inp == "healthc")
        {
            prln("Enter a scale ratio for status bar: ");
            int n_scale = pc.getInpn();
            cout<<"\n";
            c_status_bar(pc.hpmax, pc.hp, 4, n_scale, "<{", "}>", "|", "-");
        }
        else if(pc.inp == "lava") {pc.area = loc[9][1]; debugMsg(pc.area.name, 1);}
        else if(pc.inp == "strcase") {cout<<"Enter a string ->"; string str; cin >> str; cout<<strCase(str, "upper");}
        else if(pc.inp == "window") int result = StatWinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWDEFAULT);
        else if(pc.inp == "dark") Shadow(0);
        else if(pc.inp=="well") enchantedWell();
        else if(pc.inp=="bard") Bard();
        else if(pc.inp=="gatherer") Gatherer();
        else if (pc.inp=="allways"&&admin)
        {
            loc[5][9].north = true;
            loc[2][6].east = true;
            loc[1][4].east = true;
        }
        else if(pc.inp=="setkarma"&&admin)
        {
            cout<<"Set karma: ";
            pc.karma=check_int();
        }
        else if(pc.inp=="cards")
            dekara();
        else if(pc.inp=="write_to_file")
        {
            saveGameState();
            writeState();
        }
        else if(pc.inp=="lost") theLost(0);
        else if(pc.inp=="weak") theWeak();
        else if(pc.inp=="sickly_sam") theSick();
        else if(pc.inp=="needful_nelly") theNeedful();
        else if(admin&&pc.inp=="bee")
        {
            pickup(eq[64]);
            int x=searchinv(64);
            pc.inv[x].ench="Bee";
            pc.inv[x].enchb=16;
            pc.inv[x].subt="pet";
            cout<<"You've got a bee.\nGood for you.\n";
        }
        else if(admin&&pc.inp=="masterchef")
        {
            pc.cklvl=20;
        }
        else if(admin&&pc.inp=="pie_stuff")
        {
            pickup(eq[getItemID("none", "flour")]);
            pickup(eq[getItemID("raw", "meat")]);
            pickup(eq[64]);
            pc.inv[searchinvtype("container")].ench="water";
            cout<<"\nYou got the pie stuff.\n";
        }
		else if(pc.inp=="writenote") note(1);
		else if(pc.inp=="readnote") note(2);
        else if(pc.inp == "fairy_v") fairy_village();
        else if(pc.inp=="build_body")
        {
            pc.body.constructBody(0, pc.sex, pc.height, pc.wt, pc.age, pc.build, pc.eyec, pc.hairc, pc.skinc, pc.tat);
        }
        else if (pc.inp == "luci")
        {
            prln("You summon a morally grey pixie to your side.");
            pc.comp = luci;
            pc.comp.initializeAI();
            pc.comp.initializeInv();
            cout << "\nYou are now traveling with " << pc.comp.name << "\n";
        }
        else if (pc.inp == "winner") { for (int i = 0; i < 99; i++) { trophy_list[i].unlock = true; } }


        /*====================
        [-------EMOTES-------]
        ====================*/

        else if(pc.inp == "emotes")
        {
            prln("EMOTES");
            cout << "------------------\n\n";
            for (int i = 0; i < 15; i++)
            {
                cout << emotes_list[i] << "\n";
            }
        }
        else if(pc.inp=="swim") if(pc.area.water)prln("You go for a refreshing swim.\nYou cavort in the water a little."); else prln("You pretend to swim in the dirt.");
        else if(pc.inp=="smile") cout<<"\nYou smile happily.\n";
        else if(pc.inp=="laugh"||pc.inp=="chortle"||pc.inp=="chuckle") cout<<"\nYou chuckle heartily.\n";
        else if(pc.inp=="frown"||pc.inp=="grimace") cout<<"\nYou turn down the corners of your mouth unhappily.\n";
        else if(pc.inp=="cry"||pc.inp=="weep") cout<<"\nYou break down and weep miserably.\n";
        else if(pc.inp=="dance"||pc.inp=="caper") cout<<"\nYou perform a small jig.\n";
        else if(pc.inp=="pose") cout<<"\nYou strike a heroic pose.\n";
        else if(pc.inp=="jump"||pc.inp=="leap") cout<<"\nYou leap into the air. It's not very high.\n";
        else if(pc.inp=="crouch"||pc.inp=="squat") cout<<"\nYou squat back on your haunches, feeling tough.\n";
        else if(pc.inp=="scowl") cout<<"\nYou furrow your brow angrily.\n";
        else if(pc.inp=="glare") cout<<"\nYou narrow your eyes and glare venomously.\n";
        else if(pc.inp=="snap"||pc.inp=="click") cout<<"\nYou snap your fingers decisively.\n";
        else if(pc.inp=="clap"||pc.inp=="applaud") cout<<"\nYou clap your hands delightedly.\n";
        else if(pc.inp=="think"||pc.inp=="wonder"||pc.inp=="ponder"||pc.inp=="consider") cout<<"\nYou stroke your chin, pondering the secrets of the universe.\n";
        else if(pc.inp=="grin") cout<<"\nYou bare your teeth in a manic grin.\n";
        else if(pc.inp=="skip") cout<<"\nYou skip flippantly, not a care in the world.\n";
        else if(pc.inp=="sing") cout<<"\nYou belt out an old shanty, singing merrily.\n";
        else if(pc.inp=="nod") cout<<"\nYou nod your head affirmatively.\n";
        else if(pc.inp=="shake") cout<<"\nYou shake your head in denial.\n";
        else if(pc.inp=="point") cout<<"\nYou jab a finger in an important direction.\n";
        else if(pc.inp == "stand") prln("You stand up straight.");
        else if(pc.inp=="pout"||pc.inp=="sulk") cout<<"\nYou stick out your bottom lip in dejected disappointment.\n";
        else if(pc.inp=="shout"||pc.inp=="yell"||pc.inp=="bellow") cout<<"\nYou release your pent-up emotions in a deafening shout\nIt echoes back to you, begging reflection.\n";
        else if(pc.inp=="whisper") cout<<"\nYou whisper a secret to the world.\nYou feel somewhat unburdened...but not really.\n";
        else if(pc.inp=="flip"||pc.inp=="toss")
        {
            if(pc.gp<=0) {cout<<"\nYou don't even have a single coin!\n"; break;}
            cout<<"\nYou reach into your coin pouch and flip a gold coin into the air.\n\n";
            string side[2] = {"heads", "tails"};
            cout<<"It comes up " << side[ rollfz(2) ] << ".\n";
        }
        else if (pc.inp == "pray")
        {
            string s = pc.status;
            prln("You close your eyes, folding your hands together and bowing your head in silence.");
            if (s == "Terrified" || s == "Confused") pc.status = "Normal";
            if (pc.clas == "Priest" || pc.clas == "Avatar") {prln("Your connection with the light strengthens."); pc.xp++;}
            if (pc.area.name == "Temple of Light")
            {
                if ( rollfor( (pc.lck + pc.karma), 248) ) {
                    prln("A sunbeam on the surface of the fountain's bubbling waters catches your eye,\nand your body floods with gentle warmth.\nA slow smile spreads across your face.");
                    pc.lck++;
                }
                else {
                    prln("The silence of the temple eases the burdens of your soul.");
                    setKarma(pc.karma, true);
                }
            }
        }
        else if(pc.inp=="say")
        {
            cout<<"What do you want to say?\n";
            getline(cin, pc.inp);
            cout<<"\nYou say '"<<pc.inp<<"'\nin a "; if (pc.voice != "none" && pc.voice != "") cout << pc.voice; else cout << "loud, clear"; cout <<  " voice.\n";
        }
        else if(pc.inp=="wait")
		{
			int wait=rand()%3;
			string wText[3] = {"You hum a little tune.", "You tap your foot impatiently.", "You watch the clouds for awhile."};
			cout<<"\nTime passes. "<<wText[wait]<<"\n";
		}
		else if (pc.inp == "sit") prln("You sit comfortably on the ground.");


        /*------CHARACTER MENUS-------*/
        /*
            Contents:

            -View Character Description
            -Customize appearance
            -View HP, MP, and XP Bars
            -Rename Character
            -View Character Stats/Status
            -View Skill Levels
            -View Character Spells/Abilities
            -Character Gold
            -Character Quest List
            -Character Inventory
            -Character Tools
            -Trophy List/Status
        */
        else if(pc.inp=="set")
        {
            prln("Which aspect of yourself do you want to change?");
            string var = pc.getInps();
            prln("Enter a numerical value (if changing text, such as name or race, enter 0):");
            int val_n = pc.getInpn();
            prln("Enter text value (if changing a number, enter 'none'):");
            string val_s = pc.getInps();
            pc.set(var, val_n, val_s);
        }
        else if(pc.inp=="spells") showSpells();
		else if(pc.inp=="char"||pc.inp=="character" || pc.inp == "self" || pc.inp == pc.name) chars();
		else if(pc.inp=="customize") customize();
		else if(pc.inp == "name") prln("You are currently known as " + pc.name);
        else if(pc.inp=="rename"){cout<<"Enter a new name for your character:\n"; cin>>pc.inp; pc.name=pc.inp; cout<<"Name changed to "<<pc.name<<".\n";}
		else if(pc.inp=="stats"){
            if(game_settings.sound&&game_settings.display_type==0) mciSendString(sdPaper.c_str(), NULL, 0, NULL);
			pc.statscrn();}
		else if(pc.inp=="hp"||pc.inp=="health"||pc.inp=="life") {cout<<"\n"; pc.showHealth();}
		else if(pc.inp=="mp" || pc.inp =="mana" || pc.inp == "energy") {pc.showMana();}
		else if(pc.inp=="xp" || pc.inp == "experience") {pc.xpBar();}
		else if(pc.inp=="gold"||pc.inp=="gp") cout<<"\nYou have "<<pc.gp<<" gold pieces.\n";
		else if(pc.inp=="quests"||pc.inp=="questlog") questlog();
		else if(pc.inp=="inv"||pc.inp=="inventory"||pc.inp == "i") inv();
		else if(pc.inp=="tools") show_tools();
		else if(pc.inp=="trophies")
        {
			 if(admin)
             {
                 for(int x=0;x<100;x++)
                    trophy_list[x].unlock=true;
             }
             showTrophyList();
        }
        else if(pc.inp=="showcustom")
        {
            pc.showCustomCommands();
        }
        else if(pc.inp=="makecustom"||pc.inp=="custom"||pc.inp=="customcommand"||pc.inp=="custom_command")
        {
            pc.getCustomCommand();
        }

        /*-----Appearance------*/
        else if(pc.inp == "weight") cout << "\nYou weigh " << pc.wt << " pounds.\nYou are carrying " << pc.carrywt << " pounds of equipment.\n";
        else if(pc.inp == "height") cout << "\nYou are " << pc.height << "cm tall.\n";
        else if(pc.inp == "age") cout << "\nYou are " << pc.age << " years old.\n";
        else if(pc.inp == "eyes")
            {if(pc.eyec != "none") cout << "\nYou have " << pc.eyec << " eyes.\n";}
        else if(pc.inp == "hair")
            {if(pc.hairc != "none") cout << "\nYou have " << pc.hairc << " hair.\n";}
        else if(pc.inp == "skin")
           {if(pc.skinc != "none") cout << "\nYou have " << pc.skinc << " skin.\n";}

        /*-----Skill Displays-------*/
        else if(pc.inp=="mining") cout<<"\nYour mining proficiency is "<<pc.mlvl<<".\n";
        else if(pc.inp=="smithing") cout<<"\nYour smithing proficiency is "<<pc.smlvl<<".\n";
        else if(pc.inp=="woodcutting"||pc.inp=="chopping"||pc.inp=="logging"||pc.inp=="wc") cout<<"\nYour woodcutting proficiency is "<<pc.wclvl<<".\n";
        else if(pc.inp=="firemaking") cout<<"\nYour firemaking proficiency is "<<pc.fmlvl<<".\n";
        else if(pc.inp=="carving"||pc.inp=="woodworking") cout<<"\nYour woodworking proficiency is "<<pc.clvl<<".\n";
        else if(pc.inp=="hunting") cout<<"\nYour hunting proficiency is "<<pc.hlvl<<".\n";
        else if(pc.inp=="foraging") cout<<"\nYour foraging proficiency is "<<pc.frglvl<<".\n";
        else if(pc.inp=="enchanting") cout<<"\nYour enchanting proficiency is "<<pc.enchlvl<<".\n";
        else if(pc.inp=="literacy"||pc.inp=="reading"||pc.inp=="writing") cout<<"\nYour literacy proficiency is "<<pc.litlvl<<".\n";
        else if(pc.inp=="fishing") cout<<"\nYour fishing proficiency is "<<pc.flvl<<".\n";
        else if(pc.inp=="music") cout<<"\nYour musical proficiency is "<<pc.muslvl<<".\n";

        /*-----Inventory Displays-------*/
        else if(pc.inp=="weapon" || pc.inp == "weap")
            if(pc.weap.name!="empty") prln("You are wielding "+aoran(pc.weap.mat, false)+pc.weap.showName()+".");
            else prln("You are not currently wielding a weapon.");
        else if(pc.inp=="armor" || pc.inp == "arm")
            if(pc.arm.name != "empty") prln("You are wearing "+aoran(pc.arm.mat, false)+pc.arm.showName()+".");
            else prln("You are not currently wearing any armor.");
        else if(pc.inp=="accessory" || pc.inp == "acc")
            if(pc.acc.name != "empty") prln("You are quipped with "+aoran(pc.acc.mat, false)+pc.acc.showName()+".");
            else prln("You are not currently using any kind of accessory.");
        else if(pc.inp=="potions" || pc.inp == "pots")
        {
            if(!pc.area.potshop)
            cout<<"\nYou have "<<pc.hpot<<" health potions, "<<pc.mpot<<" mana potions,\nand "<<pc.apot<<" antidote potions.\n";
            else potshop();
        }
        else if(pc.inp=="arrows") cout<<"\nYou have "<<pc.arrows<<" arrows.\n";
        else if(pc.inp == "gold" || pc.inp == "gp" || pc.inp == "money") cout<<"\nYou are carrying "<<pc.gp<<" gold pieces.\n";
        else if(pc.inp=="items") pc.showInv();

        /*------STATISTICS-------*/
        else if(pc.inp == "name") prln("Your name is "+pc.name+".");
        else if(pc.inp == "status") prln("You are " + pc.status + ".");
        else if(pc.inp == "attack" || pc.inp == "atk") cout<<"\nYour attack strength is "<<pc.atk<<".\n";
        else if(pc.inp == "defense" || pc.inp == "def") cout<<"\nYour defensive strength is "<<pc.def<<".\n";
        else if(pc.inp == "strength" || pc.inp == "str") cout<<"\nYour physical strength is "<<pc.str<<".\n";
        else if(pc.inp == "dex" || pc.inp == "dexterity") cout<<"\nYour dexterity is "<<pc.dex<<".\n";
        else if(pc.inp == "int" || pc.inp == "intellect") cout<<"\nYour intellect is "<<pc.atk<<".\n";
        else if(pc.inp == "lck" || pc.inp == "luck") cout<<"Your luck is "<<pc.lck<<".\n";
        else if(pc.inp == "class") prln("You are "+aoranf(pc.clas, false)+".");
        else if(pc.inp == "race") prln("You are "+aoranf(pc.race, true)+".");
        else if(pc.inp == "story") pc.showStory();
        else if(pc.inp == "karma") { setKarma(pc.karma, 0); prln("Based on your actions, you are generally "+pc.karma_lvl); }
        else if(pc.inp == "level" || pc.inp == "lvl")
        {
            cout<<"\nYou are experience level "<<pc.lvl<<".\nProgress to next level: "<<pctOf(pc.xp, pc.xpnxt)<<"%\n";
        }
        else if (pc.inp == "equipment") pc.showEquipment();
        else if(pc.inp == "hunger" || pc.inp == "hng")  cout << "\nYou are " << pc.hunger << "% satiated.\n";
        else if(pc.inp == "thirst" || pc.inp == "thr")  cout << "\nYou are " << pc.thirst << "% hydrated.\n";
        else if(pc.inp=="kills")
        {
            cout<<"\n";
            cout<<"+---MONSTER KILLS----+\n";
            for(int ct = 0; ct<50; ct++)
            {
                if(elist[ct].kills > 0) cout<<elist[ct].name<<" kills: "<<elist[ct].kills<<"\n";
            }
        }



        /*------PLAYER ACTIONS-------*/
        /*
            Contents:

            -Interacting with inventory
            -Gathering and crafting
            -Interacting with surroundings
            -Movement and map travel
        */

		else if(pc.inp=="equip" || pc.inp == "wear" || pc.inp == "wield")
		{
			prln("Which item do you want to equip?");
			pc.showInv();

			string item = pc.getInps();
			int n_item = 0;

			//Check for both integer and string input
            if ( item.length() < 3 && isRange( 1, 10, stoi( item ) ) ) { n_item = stoi( item ); }
            else { n_item = pc.searchInvName( item ); }

            if(pc.inv[ n_item ].name != "empty") pc.equip(n_item, true, true);
            else prln("There's nothing there!");
		}
		else if(pc.inp=="unequip" || pc.inp == "remove")
		{
			cout<<"\nUnequip which item?\n";
			cout<<" (1) Weapon\n (2) Armor\n (3) Accessory\n\n";
			int num=pc.getInpn();
			pc.unequip(num, "inv");
		}
		else if(pc.inp=="drop")
			drop();
        else if(pc.inp=="sheath"||pc.inp=="stow")
        {
            if(pc.sheath&&pc.sheathed.name=="empty")
            {
                pc.sheathed=pc.weap;
                pc.weap=eq[0];
            }
            else cout<<"You can't do that now.\n";
        }
        else if(pc.inp=="draw"||pc.inp=="unsheath")
        {
            if(pc.sheath)
            {
                if(pc.sheathed.name!="empty"&&pc.weap.name=="empty")
                {
                    pc.weap=pc.sheathed;
                    pc.sheathed=eq[0];
                }
                else cout<<"You can't do that now.\n";
            }
            else cout<<"You don't have a sheath equipped!\n";
        }
		else if (pc.inp=="move"||pc.inp=="walk"||pc.inp=="travel" || pc.inp == "north" || pc.inp == "east" || pc.inp == "west" || pc.inp == "south" || pc.inp == "n" || pc.inp == "s" || pc.inp == "e" || pc.inp == "w")
			move();
		else if(pc.inp=="area")
			showarea();
		else if(pc.inp=="look" || pc.inp == "show")
			look();
		else if(pc.inp=="talk" || pc.inp == "greet")
			talk();
		else if(pc.inp=="chest" || pc.inp == "open")
        {
        if(admin==true)
        {
            cout<<"Cheat chest? Or regular type? (1 or 2)\n"; cin>>pc.inp;
            if(pc.inp=="1")
            {
                cout<<"Enter item id#: ";
                int num;
                num=check_int();;
                pc.area.loot=itemroll(1, num);
                cout<<"You kick the chest against a tree. Its contents scatter on the ground.\n";
            }
                else
                    openchest();
        }
        else openchest();
        }
		else if(pc.inp=="take" || (pc.inp == "get" && !admin) )
			{pickup(pc.area.loot);
			if ( searchinv(pc.area.loot.id) ) pc.area.loot = eq[0];}
        else if(pc.inp=="fight"||pc.inp=="swing"||pc.inp=="hit"||pc.inp=="slash"||pc.inp=="strike")
        {
            if(pc.area.foe.alive==true)
				combat();
			else
                pc.weap.showAction("none");
        }
		else if(pc.inp=="hpot")
			hpotion();
		else if(pc.inp=="mpot")
			mpotion();
		else if(pc.inp=="apot")
			apotion();
		else if(pc.inp=="cast")
        {
            cout<<"\nCast which spell?\n";
            cin>>pc.inp;
			noncomcast(pc.inp, 0);
        }

        /*------CRAFTING/GATHERING-------*/

        else if(pc.inp=="forage") forage();
        else if(pc.inp=="mix") mix();
        else if(pc.inp=="dye") dye();
		else if(pc.inp=="chop") wc();
		else if(pc.inp=="mine") mine();
		else if(pc.inp=="enchant") enchant();
		else if(pc.inp=="carve") carve();
		else if(pc.inp=="sew") sew();
		else if(pc.inp=="cook") {pc.showInv(); cook();}
        else if(pc.inp=="bake") bake();
		else if(pc.inp=="fish") fish();
		else if(pc.inp=="shear") shear();
		else if(pc.inp=="smith" || pc.inp == "smithy") smith();
		else if(pc.inp=="temper")
        {
            if(pc.area.smithy) temper();
            else cout<<"You need a full-blown forge to temper an item.\n";
        }
		else if(pc.inp=="fire")
        {
            //Check for quest triggers
            int s_torch=searchinv(207);
            int f_torch=searchinv(208);

            if(pc.area.name=="Dark Cave"&&loc[6][7].npcs[1].q.lvl==2&&s_torch>0)
            {
                cout<<"You lean down and light the beacon using the Sacred Torch.\nBright white flame blossoms upward, and somewhere in the caves something shrieks angrily.\n";
                cout<<"\n  ->You've lit the beacon and ensured safe passage. Sister Suni would probably like to know.\n";
                loc[6][7].npcs[1].questcond=true;
            }
            else if(pc.area.name=="Vampsect Lair"&&loc[2][2].npcs[0].q.lvl==3&&f_torch>0)
            {
                cout<<"You lean down and light the pile of pine-soaked logs with the foul torch.\nImmediately thick plumes of green-tinged smoke billow up, filling the area.\nYou hear several minutes of angry buzzing and then silence, as one by one they\ndrop from the sky, splattering into the swamp and sinking below the surface.\n\nYou watch with mixed feelings as the flame catches a nearby peat bog, eradicating the nest in a messy explosion.\n";
                cout<<"\n  ->You've solved the Shaman's dilemma. He'd probably want to know.\n";
                loc[2][2].npcs[0].questcond=true;
            }
            else fire();
        }
		else if(pc.inp=="harvest")
		{
			if(pc.area.crops==true)
			{
				int crops=rand()%4;
				cout<<"You walk down the rows looking for anything to harvest.\n";
				if(crops>0)
				{
				    int ctypes[3]={getItemID("none", "carrot"), getItemID("raw", "potato"), getItemID("raw", "corn")};
					for(int ct=1;ct<=crops;ct++)
                    {
                        if(pc.area.id==41) pickup(eq[85]);
                        if(pc.area.id==32) pickup(eq[ctypes[rand()%3]]);
                        pc.area.crops=false;
                        loc[pc.area.x][pc.area.y].crops=false;
                    }
				}
				else
					cout<<"There's nothing ready to harvest.\n";
			}
			else
				cout<<"There's nothing to harvest...\nmaybe you could plant something?\n";
		}
		else if(pc.inp=="pick" || pc.inp == pc.area.loc_flora.name)
		{
		    if(pc.area.loc_flora.name!="null"&&pc.area.loc_flora.item!=0)
            {
                cout<<"You reach out and pick the "<<pc.area.loc_flora.name<<".\n";
                pickup(eq[pc.area.loc_flora.item]);
                pc.area.loc_flora=plant[0];
            }
			else if(pc.area.herbs==true)
			{
                if(pc.area.name=="Forest"||pc.area.name=="Sparse Forest"||pc.area.name=="Forest Path"||pc.area.name=="Ancient Forest")
                {
                    cout<<"You lean down and strip the leaves from a medicinal plant.\n";
                    pickup(eq[62]);
                }
                else if(pc.area.name=="Tundra")
                {
                    cout<<"You lean down and pick the bundle of Snowflowers.\n";
                    pickup(eq[103]);
                }
                else if(pc.area.name=="Lava Plain")
                {
                    cout<<"You snap the brittle stalk, pocketing the Torchweed blossom.\n";
                    pickup(eq[104]);
                }
                    pc.area.herbs=false;
			}
			else
				cout<<"You pluck a few blades of grass, then, let them blow away in the wind.\n";
		}
		else if(pc.inp=="fill") fill();

        else if(pc.inp=="rest"||pc.inp=="sleep") rest();
		else if(pc.inp=="eat") {pc.showInv(); eat();}
		else if(pc.inp=="drink") {pc.showInv(); drink();}
        else if(pc.inp=="read") {read();}
        else if(pc.inp=="play")
        {
            play();
        }
        else if(pc.inp=="poison")
        {
            poison();
        }
		else if(pc.inp=="bank")
        {
            cout<<"You enter the bank between fluted marble columns and approach a booth.\n";
            offshore();
        }

		else if(pc.inp=="map")
			show_map();
		else if(pc.inp=="legend")
			legend();
		else if(pc.inp=="guildshop")
		{
			switch(pc.area.id)
			{
				case 34:
					if(pc.clas=="Archon")
                        guildshop(2);
                    else cout<<"Azelfoff wags a crooked finger, cackling.\n'You don't look like an Archon to me!'\n";
				break;
				case 36:
					if(pc.clas=="Avatar")
                        guildshop(3);
                    else cout<<"Suni smiles apologetically. 'I'm sorry, but I can only sell these items to Temple attendants.'\n";
				break;
				case 54:
					if(pc.clas=="Paladin")
                        guildshop(0);
                    else cout<<"Emiria merely shakes her head, her expression unreadable.\n";
				break;
				case 56:
					if(pc.clas=="Assassin")
                        guildshop(1);
                    else cout<<"Hakon laughs gruffly. 'Nice try, stranger, but we don't deal with just anybody.'\n";
				break;
				default:
					cout<<"You look about for an equipment vendor but find nothing.\n";
				break;
			}
		}
		else if(pc.inp=="quartermaster"||pc.inp=="Quartermaster")
        {
            if(pc.area.name=="Castle Courtyard")
                quartermaster();
            else
                cout<<"There's nobody like that here.\n";
        }
		else if(pc.inp=="inn")
        {
            if(pc.area.name=="Village"||pc.area.name=="Elven Village"||pc.area.name=="Winterhold"||pc.area.name=="City Gate East")
                inn();
            else cout<<"You look around for a place to grab a drink and rest, but there are no inns or taverns nearby.\n";
        }
		else if(pc.inp=="armory")
		{
			if(pc.area.armory==true)
				armory();
			else
				cout<<"You look about for an equipment vendor but find nothing.\n";
		}
		else if(pc.inp=="merchant")
		{
			if(pc.area.toolshop==true)
				toolshop();
			else
				cout<<"You look about for a traveling merchant but find nothing.\n";
		}
		else if(pc.inp=="potions")
		{
			if(pc.area.potshop==true)
				potshop();
			else
				cout<<"You look about for a potion shop but find nothing.\n";
		}
		else if(pc.inp=="fishmarket")
		{
			if(pc.area.name=="City Gate West")
			{
				fishop();
			}
			else
				cout<<"There's no fish market here!\n";
		}
		else if(pc.inp=="fencer")
        {
            if(pc.area.name=="Grassland Caravan")
            {
                fencer();
            }
            else cout<<"You don't see anyone like that here.\n";
        }
		else if(pc.inp=="gemshop")
		{
			if(pc.area.id==20)
				magshop();
			else cout<<"You don't see anyone selling gems.\n";
		}
		else if(pc.inp=="bakery")
        {
            if(pc.area.name=="City Center") create_shop(0, "Bakery");
            else cout<<"You spend a minute looking for somewhere to buy a pastry.\nAlas! No joy.\n";
        }
        else if(pc.inp=="library")
        {
            if(pc.area.name=="City Center") library();
            else cout<<"You don't think you'll see any books for a while.\n";
        }
        else if(pc.inp=="mill"||pc.inp=="windmill")
        {
            if(pc.area.name=="Windmill"){mill();}
            else cout<<"There's no windmill here.\n";
        }
        else if(pc.inp=="path")
        {
            if (pc.area.name == "Frost's Eyrie") mountainPath();
            else prln("You don't see any paths you want to follow.");
        }
        else if(pc.inp == "brooch" && pc.fairy_missive == 2){
            prln("You take out the brooch given to you by the Fae princess Drin'r.\nAs you gaze into its depths, you feel a ghostly forest pressing against you.");
            prln("Enter the Fairy Village? (Y / N)");
            if ( inpAffirm( pc.getInps() ) ) fairy_village();
        }
        else if(pc.inp == "ring" && pc.fairy_missive == 3){
            prln("You take out the ring given to you by the Fae Queen.\nYou whisper your command; with a gentle burst of moonlit motes you find yourself between two mossy trunks.");
            prln("Enter the Fairy Village? (Y / N)");
            if ( inpAffirm( pc.getInps() ) ) fairy_village();
        }
        else if(pc.inp=="sick")
        {
            if(m_data.sick_nearby) theSick();
            else cout<<"You don't see anyone nearby that needs healing.\n";
        }
        else if(pc.inp=="needful")
        {
            if(m_data.needful_nearby) theNeedful();
            else cout<<"You don't see anyone nearby that needs an item.\n";
        }
		else if(pc.inp=="skills")
			pc.showSkills();
		else if(pc.inp=="hunt")
			hunt();
        else if(pc.inp=="catch")
        {
            catch_npc();
        }
        else if(pc.inp=="release")
        {
            release();
        }
		else if(pc.inp=="sow")
		{
			int x=searchinv(298);
			if(pc.area.name=="Farmland"&&x!=0)
			{
				cout<<"You sow the seeds as neatly as you can.\n";
				pc.carrywt-=pc.inv[x].wt;
				pc.inv[x]=eq[0];
				if(loc[2][6].npcs[0].q.comp==false)
				{
					loc[2][6].npcs[0].questcond=true;
				}
				loc[1][6].plantd=true;
			}
			else
				cout<<"You can't do that now!\n";
		}
		else if (pc.inp=="companion")
			compTalk();


        /*===========================
        [-------ADMIN / DEBUG-------]
        ============================*/

		else if (pc.inp=="admin")
        {
            cout<<"Enter admin password:\n";
            cin>>pc.inp;
            if(pc.inp=="hitlerdidnothingwrong")
            {
                admin=true;
                cout<<"\nAdmin mode initiated.\n";
            }
            else
                cout<<"Nope! You were close, though!\n";
        }

        /*----ADMIN COMMANDS----*/
        /*
            -Access or change game, player information
            -Access game data, including object IDs etc.
            -Set player stats
            -Spawn items, enemies or NPCs
        */

        else if(admin==true){
		if(pc.inp=="maptest")
			show_map();
        else if(pc.inp == "getspellid")
        {
            cout << "Spell: ";
            cin >> pc.inp;

            for (int i = 0; i < 69; i++) { if(splist[i].name == pc.inp) {cout << "ID is " << i << "\n"; break;} }
        }
        else if (pc.inp=="print_env")
        {
            print_env_options();
        }
        else if(pc.inp=="set_fae")
        {
            int x, y;
            cout<<"Set x and y coordinates for Fae Court.\n";
            cout<<"X: ";
            x=check_int();
            cout<<"Y: ";
            y=check_int();
            m_data.fairy_loc=loc[x][y].name;
            cout<<"Set level for fairy quest: ";
            pc.fairy_missive=check_int();

        }
        else if(pc.inp=="fairy") Faerie();
        else if(pc.inp=="weak") theWeak();
        else if(pc.inp=="print_area_id")
        {
            ofstream area_id_list;
            area_id_list.open("../Reference/TSF_Area_ID_List.txt");
            for(int y=1;y<11;y++)
            {
                for(int x=1;x<11;x++)
                {
                    area_id_list<<loc[x][y].name<<" -- "<<loc[x][y].id<<"\n";
                }
            }
            area_id_list.close();
        }
		else if(pc.inp=="setplayer")
		{
			cout<<"Enter player information. Enter skills & tools with 'skillset', items with 'get' & item id.\n\n";
			cout<<"Level: ";
			cin>>pc.lvl;
			cout<<"Class: ";
			cin>>pc.clas;
			cout<<"HP: ";
			cin>>pc.hpmax;
			pc.hp=pc.hpmax;
			cout<<"MP: ";
			cin>>pc.mpmax;
			pc.mp=pc.mpmax;
			cout<<"Str: ";
			cin>>pc.str;
			cout<<"Dex: ";
			cin>>pc.dex;
			cout<<"Int: ";
			cin>>pc.intl;
			cout<<"Lck: ";
			cin>>pc.lck;
			cout<<"Gold: ";
			cin>>pc.gp;
		}
		else if(pc.inp=="killset")
        {
            int x;
            cout<<"Set kills for which enemy? (enter number.)\n";
            cin>>x;
            cout<<"Set kills for "<<elist[x].name<<" to how many?\n";
            int y; cin>>y;
            elist[x].kills=y;
            cout<<"Kill count for "<<elist[x].name<<" set to "<<y<<".\n";
        }
		else if(pc.inp=="skillset")
		{
			cout<<"For tools: enter '1' for true, '0' for false.\n\n";
			cout<<"Woodcutting: ";
			cin>>pc.wclvl;
			cout<<"Axe: ";
			cin>>pc.axe;
			cout<<"Firemaking: ";
			cin>>pc.fmlvl;
			cout<<"Tinderbox: ";
			cin>>pc.flvl;
			cout<<"Cooking: ";
			cin>>pc.cklvl;
			cout<<"Hunting: ";
			cin>>pc.hlvl;
			cout<<"Fishing: ";
			cin>>pc.flvl;
			cout<<"Fishing Rod: ";
			cin>>pc.frod;
			cout<<"Mining: ";
			cin>>pc.mlvl;
			cout<<"Pickaxe: ";
			cin>>pc.pick;
			cout<<"Smithing: ";
			cin>>pc.smlvl;
			cout<<"Hammer ";
			cin>>pc.ham;
			cout<<"Carving: ";
			cin>>pc.clvl;
			cout<<"Chisel: ";
			cin>>pc.chis;
			cout<<"Sewing: ";
			cin>>pc.swlvl;
			cout<<"Needle & Thread: ";
			cin>>pc.need;
			cout<<"Enchanting: ";
			cin>>pc.enchlvl;
		}
		else if(pc.inp == "zezima")
        {
            colSet(COL_RED, "\nred: wave: 92 is half of 99\n");
            pc.wclvl = 99;
            pc.axe = true;
            pc.fmlvl = 99;
            pc.tbox = true;
            pc.mlvl = 99;
            pc.pick = true;
            pc.smlvl = 99;
            pc.ham = true;
            pc.flvl = 99;
            pc.frod = 2;
            pc.bait = 20;
            pc.shears = true;
            pc.clvl = 99;
            pc.chis = true;
            pc.swlvl = 99;
            pc.need = true;
            pc.cklvl = 99;
            pc.enchlvl = 99;
            pc.hlvl = 99;
        }
		else if(pc.inp=="exitadmin")
		{
			admin=false;
			cout<<"Logged off.\n";
		}
		else if(pc.inp=="knowledge!=")
			pc.xp+=pc.xpnxt;
		else if(pc.inp=="upallnight")
			pc.lck+=20;
		else if(pc.inp=="smithy")
			pc.area.smithy=true;
		else if(pc.inp=="unlock")
		{
			int num;
			num=check_int();;
			splist[num].unlock=true;
		}
		else if(pc.inp=="get")
		{
			cout<<"Get which item? (id#)\n";
			int x;
			cin>>x;
			pickup(eq[x]);
		}
		else if(pc.inp=="getid")
		{
			string mat, name;
			int id;
			cout<<"Material: ";
			cin>>mat;
			cout<<"Name: ";
			cin>>name;
			id=getItemID(mat, name);
			cout<<"Item ID is: "<<id<<endl;
		}
		else if(pc.inp=="summon")
		{
			cout<<"Summon what (id#)?\n";
			int num;
			num=check_int();;
			pc.area.foe=elist[num];
			cout<<"You open a mystical portal, and summon a "<<pc.area.foe.name<<"\nfrom elsewhere in the world.\n";
			pc.area.foe.alive=true;
		}
		else if(pc.inp=="summon_tom")
        {
            pc.area.npcs[2]=cast_npc[2];
            cout<<"DEBUG: NPC number 3 is "<<pc.area.npcs[2].name<<"\n";
        }
        else if(pc.inp=="set_tom")
        {
            pc.area.npcs[2].q.setProcQuest();
            cout<<"DEBUG: Tom's new quest is to find a "<<pc.area.npcs[2].q.name<<"\n";
            pc.area.npcs[2].appendQ();
            cout<<"DEBUG: All systems functional.\n";
        }
        else if(pc.inp=="print_item_id")
        {
            fstream itemid;
            itemid.open("item_id_list.txt");
            if(!itemid.is_open())
            {
                cout<<"Error opening file\n";
            }
            for(int x=0;x<300;x++)
            {
                itemid<<eq[x].mat<<" "<<eq[x].name<<" -- "<<x<<"\n";
            }
            itemid.close();
        }
		else if(pc.inp=="teleport")
		{
			cout<<"Enter x: ";
			int x;
			cin>>x;
			cout<<"Enter y: ";
			int y;
			cin>>y;
			if(x<11&&y<11)
			{
				cout<<"You touch two fingers to your forehead and\nvanish, instantly transmitting to ";
				pc.area=loc[x][y];
				cout<<endl;
				showarea();
			}
			else cout<<"Must teleport on this spectral plane only please\n";
		}
		else if(pc.inp == "quickench")
		{
		    pc.showInv();
			cout << "Enter inv #:\n";
			int num;
			num = pc.getInpn();
			cout << "Enter enchantment: \n";

			cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

			string ench;
			getline(cin, ench);

			pc.inv[num].ench = ench;
		}
		else if(pc.inp=="havesomeclass")
		{
			cout<<"Change class to:\n";
			cin>>pc.inp;
			pc.clas=pc.inp;
		}
		else if (pc.inp=="plotarmor")
        {
            pc.hpmax=10000;
            pc.mpmax=1000;
            pc.mp=pc.mpmax;
            pc.hp=pc.hpmax;
            pc.def=20;
        }
		else if(pc.inp=="hire")
		{
			cout<<"Hire whom?\n";
			cin>>pc.inp;
			if(pc.inp=="luz")
            {
                pc.comp=luz;
            }
			else if(pc.inp=="grognak")
				{pc.comp=grognak;}
            else if(pc.inp=="Luci")
                {pc.comp=luci;}
            else if(pc.inp=="frost")
            {
                pc.comp=frost;
            }
			else cout<<"You can't recruit that person.\nIf that even is a person.\n";

			if(pc.comp.name!="empty") {pc.comp.initializeAI(); pc.comp.initializeInv();}
		}
        else if(pc.inp=="tent")
        {
            pc.hp=pc.hpmax;
            pc.mp=pc.mpmax;
            cout<<"You pitch a tent in the wilderness and rest.\n";
        }
        else if(pc.inp=="house")
        {
            pc.area.loc_bldg=house;
            pc.area.loc_bldg.complete=true;
        }
        else if(pc.inp=="midaswell")
        {
        	cout<<"You brush against a terrain object, and\nto your surprise it collapses into a\npile of shiny gold coins.\nThe next few minutes are full of gleeful\ngreed as you transform much of your\nsurroundings into currency.\nGained 10,000 gold.\n";
        	pc.gp+=10000;
        }
        else if(pc.inp == "wolf") {elist[3].showInfo();}
        else if(pc.inp=="setday")
        {
            cout<<"Set day to (#): ";
            int num; num=check_int();
            day=num;
        }
        else if(pc.inp=="bakery")
        {
            create_shop(0, "Bakery");
        }
        else if(pc.inp=="nothreat")
        {
            pc.status="Terrifying";
        }
        else if (pc.inp=="comp_level")
        {
            pc.comp.xp+=pc.comp.xpnxt;
        }
        else if(pc.inp=="qlist")
        {
            for(int x=0;x<25;x++)
            {
                cout<<x<<": "<<qlog[x].name;
            }
        }

        /*DEBUG*/

        else if (pc.inp == "debug_add")
        {
            addDebug();
        }
        else if(pc.inp == "debug_read")
        {
            readDebug();
        }

    }//If admin==true

		else
			if ( pc.inp != "quit" && !disable_error_flag ) cout << "I don't understand...type 'help' for command list.\n";
		}
	}while(game_settings.is_playing);
}

