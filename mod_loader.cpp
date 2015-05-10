#include "mod_loader.h"

#include <cmath>
#include "func.h"

using namespace std;
using namespace Debugger;

void Mod::load_item_info(const string& filename){//loads object info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	general_items.clear();
	//seen_item_text.clear();
	//quick_keys.clear();


	fil = fopen(filename.c_str(),"rt");
	if(fil){
		general_item_base temp_item;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//object name
			temp_item.name=rivi;
			debug->debug_output("Load "+temp_item.name,Action::START,Logfile::STARTUP);

			temp_item.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//identifier
			temp_item.item_class=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_item.weight=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_item.description=rivi;
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_item.event_text=rivi;
			temp_item.size=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_item.show_on_radar=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_item.show_on_radar_particle=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_item.visible_in_inventory=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			//load texture
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_item.texture_name=rivi;
			temp_item.texture=resources->load_texture(rivi,mod_name);
			temp_item.wielded_script=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_item.wielded_disabling_script=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_item.show_condition_help=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));

			//wield slots
			temp_item.wield_slots.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_wield_slots")!=0){
				temp_item.wield_slots.push_back(atoi(rivi));
			}

			//uses
			temp_item.effects.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects_block")!=0){
				item_effect_base temp_effect;


				temp_effect.effect.event_text=rivi;
				temp_effect.effect.event_failure_text=stripped_fgets(rivi,sizeof(rivi),fil);
				temp_effect.effect.sound=-1;
				temp_effect.effect.sound_name=stripped_fgets(rivi,sizeof(rivi),fil);
				if(temp_effect.effect.sound_name!="none")temp_effect.effect.sound=resources->load_sample(rivi,2,mod_name);//hit sound
				temp_effect.effect.vanish_after_used=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.use_key=string(stripped_fgets(rivi,sizeof(rivi),fil)).at(0);
				temp_effect.quick_key=string(stripped_fgets(rivi,sizeof(rivi),fil)).at(0);
				/*if(temp_effect.quick_key!=0){
					quick_uses key;
					key.item_type=temp_item.identifier;
					key.key=temp_effect.quick_key;
					quick_keys.push_back(key);
				}*/

				//conditions
				temp_effect.effect.conditions.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_conditions")!=0){
					condition temp_effect_condition;

					temp_effect_condition.condition_number=atoi(rivi);
					temp_effect_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effect.conditions.push_back(temp_effect_condition);
				}

				//effects
				temp_effect.effect.effects.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects")!=0){
					effect temp_effect_effect;

					temp_effect_effect.effect_number=atoi(rivi);
					temp_effect_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effect.effects.push_back(temp_effect_effect);
				}

				temp_item.effects.push_back(temp_effect);
			}

			//combinations
			temp_item.combinations.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_combinations")!=0){
				combines temp_combination;
				temp_combination.discard_this=strtobool(rivi);
				temp_combination.combines_with=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_combination.discard_that=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_combination.combine_puzzle_difficulty=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_combination.can_be_broken_up=atoi(stripped_fgets(rivi,sizeof(rivi),fil));


				temp_combination.combine_results.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_items_given")!=0){
					combines::combine_results_base temp_result;
					temp_result.combines_to=atoi(rivi);
					temp_result.combines_amount=atoi(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_combination.combine_results.push_back(temp_result);
				}


				temp_item.combinations.push_back(temp_combination);
			}



			//if the identifier is bigger than current list size, increase list size
			temp_item.dead=true;
			while(general_items.size()<temp_item.identifier+1){
				//put in dummy elements
				general_items.push_back(temp_item);
				//seen_item_text.push_back(false);
			}
			temp_item.dead=false;
			general_items[temp_item.identifier]=temp_item;
			//seen_item_text[temp_item.identifier]=false;



			debug->debug_output("Load "+temp_item.name,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);


	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}


void Mod::load_object_info(const string& filename){//loads object info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[800];

	//int object_infos=0;

	general_objects.clear();

/*	fil = fopen(filename.c_str(),"rt");
	if(fil){
		general_objects_base temp_object;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){

			temp_object.name=rivi;
			temp_object.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			stripped_fgets(rivi,sizeof(rivi),fil);//texture name
			//load texture if it's not loaded
			temp_object.texture_name=rivi;
			temp_object.texture=resources->load_texture(rivi);

			temp_object.base_size=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//base_size
			temp_object.vary_size=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//vary_size
			temp_object.transparent=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//transparency
			temp_object.layer=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//layer
			temp_object.swing=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//swing
			temp_object.collision_type=0;
			temp_object.passable_radius=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//passable_radius
			temp_object.collision_parameter0=temp_object.passable_radius;
			temp_object.collision_parameter1=0;


			if(temp_object.passable_radius==0)temp_object.collision_type=-1;

			//if the identifier is bigger than current list size, increase list size
			while(general_objects.size()<temp_object.identifier+1){
				//put in dummy elements
				general_objects.push_back(temp_object);
			}
			general_objects[temp_object.identifier]=temp_object;
		}
	}
	fclose(fil);*/

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		general_objects_base temp_object;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){

			temp_object.name=rivi;
			temp_object.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			//stripped_fgets(rivi,sizeof(rivi),fil);//texture name
			//load texture if it's not loaded
			//temp_object.texture_name=rivi;
			//temp_object.texture=resources->load_texture(rivi,mod_name);


			temp_object.animation_frames.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_animation_frames")!=0){
				object_animation_frame_base temp_frame;

				temp_frame.texture=resources->load_texture(rivi,mod_name);
				temp_frame.texture_name=rivi;
				temp_frame.time=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_object.animation_frames.push_back(temp_frame);
			}

			temp_object.base_size=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//base_size
			temp_object.vary_size=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//vary_size
			temp_object.transparent=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//transparency
			temp_object.layer=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//layer
			temp_object.swing=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//swing
			temp_object.get_transparent=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.provide_shade=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.collision_type=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.collision_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.collision_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.blocks_vision=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.stops_bullets=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));


			temp_object.passable_radius=temp_object.collision_parameter0;

			//if the identifier is bigger than current list size, increase list size
			temp_object.dead=true;
			while(general_objects.size()<temp_object.identifier+1){
				//put in dummy elements
				general_objects.push_back(temp_object);
			}
			temp_object.dead=false;
			general_objects[temp_object.identifier]=temp_object;
		}
	}
	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);






	//save
	/*
	filename=filename+'+';
	fil = fopen(filename.c_str(),"wt");
	general_objects_base temp_item;
	int a,b;

	for(a=0;a<general_objects.size();a++){
		temp_item=general_objects[a];
		if(temp_item.dead)continue;

		fprintf(fil, "%s;//name---------------------------------------\n", temp_item.name.c_str());
		fprintf(fil, "  %d;//identifier\n", temp_item.identifier);
		fprintf(fil, "  begin_animation_frames;\n");
		for(b=0;b<temp_item.animation_frames.size();b++){
			fprintf(fil, "    %s;//texture name\n", temp_item.animation_frames[b].texture_name.c_str());
			fprintf(fil, "    %f;//time to display frame\n", temp_item.animation_frames[b].time);

		}
		fprintf(fil, "  end_animation_frames;\n");
		//fprintf(fil, "  %s;//texture name\n", temp_item.texture_name.c_str());
		fprintf(fil, "  %f;//size*128\n", temp_item.base_size);
		fprintf(fil, "  %f;//vary size\n", temp_item.vary_size);
		fprintf(fil, "  %d;//transparent\n", temp_item.transparent);
		fprintf(fil, "  %d;//layer\n", temp_item.layer);
		fprintf(fil, "  %d;//does it sway with the wind\n", temp_item.swing);
		fprintf(fil, "  %d;//gets transparent when player near\n", temp_item.get_transparent);
		fprintf(fil, "  %d;//provides shade for player\n", temp_item.provide_shade);
		fprintf(fil, "  %d;//collision detection type, -1=none, 0=circle radius parameter0, 1=polygon parameter0 from polygons.dat size parameter1\n", temp_item.collision_type);
		fprintf(fil, "  %f;//collision detection parameter0\n", temp_item.collision_parameter0);
		fprintf(fil, "  %f;//collision detection parameter1\n", temp_item.collision_parameter1);
		fprintf(fil, "  %d;//Block vision. If 1, enemies and player cannot see other creatures through it. Use sparingly, this slows the AI down.\n", temp_item.blocks_vision);
		fprintf(fil, "  %d;//Stops bullets. Use sparingly as this can slow the game down a lot.\n", temp_item.stops_bullets);


	}
	fprintf(fil, "end_of_file;\n");

	fclose(fil);
	*/
}



void Mod::load_creature_info(const string& filename){//loads object info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];


	general_creatures.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		general_creatures_base temp_creature;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//object name
			temp_creature.name=rivi;
			debug->debug_output("Load "+temp_creature.name,Action::START,Logfile::STARTUP);

			temp_creature.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.creature_class=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			stripped_fgets(rivi,sizeof(rivi),fil);//texture name
			//load texture if it's not loaded
			temp_creature.texture=resources->load_texture(rivi,mod_name);
			temp_creature.texture_name=rivi;
			temp_creature.layer=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.corpse_item=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.corpse_item_amount=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.close_combat=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.particle_on_radar=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.AI_tactics[0]=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.AI_tactics[1]=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.footsteps=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.die_after_seconds=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.eat_item=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.size=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//size
			temp_creature.weight=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//size
			temp_creature.movement_speed=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//speed
			temp_creature.minimum_movement_speed=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.leg_animation_speed=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.turn_speed=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//speed
			temp_creature.inertia=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//speed
			temp_creature.hide_behind_walls=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.weapon=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//weapon
			temp_creature.weapon_x=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//weapon_x
			temp_creature.weapon_y=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//weapon_y
			temp_creature.blood=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.bars_visible=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.death_animation_speed=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.can_change_area=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.AI_hear_range=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.AI_see_range=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_creature.AI_see_angle=atof(stripped_fgets(rivi,sizeof(rivi),fil));


			temp_creature.footstep_sounds.clear();
			temp_creature.footstep_sound_names.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_footstep_sounds")!=0){
				temp_creature.footstep_sounds.push_back(resources->load_sample(rivi,2,mod_name));
				temp_creature.footstep_sound_names.push_back(rivi);
			}


			temp_creature.hit_sound.clear();
			temp_creature.hit_sound_names.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_hit_sounds")!=0){
				temp_creature.hit_sound.push_back(resources->load_sample(rivi,2,mod_name));
				temp_creature.hit_sound_names.push_back(rivi);
			}

			temp_creature.die_sound.clear();
			temp_creature.die_sound_names.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_die_sounds")!=0){
				temp_creature.die_sound.push_back(resources->load_sample(rivi,2,mod_name));
				temp_creature.die_sound_names.push_back(rivi);
			}

			//eat block
			temp_creature.eat_block.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_eat_block")!=0){
				general_creatures_base::effect_block temp_effect;

				//conditions
				temp_effect.conditions.clear();
				//stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_conditions")!=0){
					Mod::condition temp_effect_condition;

					temp_effect_condition.condition_number=atoi(rivi);
					temp_effect_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.conditions.push_back(temp_effect_condition);
				}
				//effects
				temp_effect.effects.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects")!=0){
					Mod::effect temp_effect_effect;

					temp_effect_effect.effect_number=atoi(rivi);
					temp_effect_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effects.push_back(temp_effect_effect);
				}

				temp_creature.eat_block.push_back(temp_effect);
			}

			//death block
			temp_creature.death_block.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_death_block")!=0){
				general_creatures_base::death_effect_block temp_effect;

				temp_effect.death_type=atoi(rivi);

				//conditions
				temp_effect.conditions.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_conditions")!=0){
					Mod::condition temp_effect_condition;

					temp_effect_condition.condition_number=atoi(rivi);
					temp_effect_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.conditions.push_back(temp_effect_condition);
				}
				//effects
				temp_effect.effects.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects")!=0){
					Mod::effect temp_effect_effect;

					temp_effect_effect.effect_number=atoi(rivi);
					temp_effect_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effects.push_back(temp_effect_effect);
				}

				temp_creature.death_block.push_back(temp_effect);
			}

			//hit block
			temp_creature.hit_block.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_hit_block")!=0){
				general_creatures_base::effect_block temp_effect;

				//conditions
				temp_effect.conditions.clear();
				//stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_conditions")!=0){
					Mod::condition temp_effect_condition;

					temp_effect_condition.condition_number=atoi(rivi);
					temp_effect_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.conditions.push_back(temp_effect_condition);
				}
				//effects
				temp_effect.effects.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects")!=0){
					Mod::effect temp_effect_effect;

					temp_effect_effect.effect_number=atoi(rivi);
					temp_effect_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effects.push_back(temp_effect_effect);
				}

				temp_creature.hit_block.push_back(temp_effect);
			}

			//timed block
			temp_creature.timed_block.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_timed_block")!=0){
				general_creatures_base::timed_effect_block temp_effect;

				temp_effect.interval=atoi(rivi);

				//conditions
				temp_effect.conditions.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_conditions")!=0){
					Mod::condition temp_effect_condition;

					temp_effect_condition.condition_number=atoi(rivi);
					temp_effect_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.conditions.push_back(temp_effect_condition);
				}
				//effects
				temp_effect.effects.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects")!=0){
					Mod::effect temp_effect_effect;

					temp_effect_effect.effect_number=atoi(rivi);
					temp_effect_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effects.push_back(temp_effect_effect);
				}

				temp_creature.timed_block.push_back(temp_effect);
			}

			//specialties
			temp_creature.specialties.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_specialties")!=0){
				general_creatures_base::creature_specialty temp_specialty;


				//temp_specialty.description="";
				temp_specialty.description=rivi;
				temp_specialty.number=atoi(stripped_fgets(rivi,sizeof(rivi),fil));

				//temp_specialty.number=atoi(rivi);

				temp_specialty.parameter0=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_specialty.parameter1=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_specialty.parameter2=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_specialty.parameter3=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_creature.specialties.push_back(temp_specialty);
			}

			//if the identifier is bigger than current list size, increase list size
			temp_creature.dead=true;
			while(general_creatures.size()<temp_creature.identifier+1){
				//put in dummy elements
				general_creatures.push_back(temp_creature);
			}
			temp_creature.dead=false;
			general_creatures[temp_creature.identifier]=temp_creature;


			debug->debug_output("Load "+temp_creature.name,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);

	debug->debug_output("Load file " + filename,Action::END,Logfile::STARTUP);


	//save
	/*
	int i,b,c;
	filename=filename+'+';
	fil = fopen(filename.c_str(),"wt");
	general_creatures_base temp_item;
	for(int a=0;a<general_creatures.size();a++){
		temp_item=general_creatures[a];
		if(temp_item.dead)continue;

		fprintf(fil, "%s;//name---------------------------------------\n", temp_item.name.c_str());
		fprintf(fil, "  %d;//identifier\n", temp_item.identifier);
		fprintf(fil, "  %d;//class\n", temp_item.creature_class);
		fprintf(fil, "  %s;//texture\n", temp_item.texture_name.c_str());
		fprintf(fil, "  %d;//draw layer\n", temp_item.layer);
		fprintf(fil, "  %d;//corpse item number in items.dat, -1=nothing, -2=whole corpse vanishes when dead\n", temp_item.corpse_item);
		fprintf(fil, "  %d;//corpse item amount\n", temp_item.corpse_item_amount);
		fprintf(fil, "  %d;//attack type, 0=shooting attack, 1=close combat\n", temp_item.close_combat);
		fprintf(fil, "  %d;//particle to show on radar, -1=none\n", temp_item.particle_on_radar);
		fprintf(fil, "  %d;//primary AI tactic from AI_tactic.dat\n", temp_item.AI_tactics[0]);
		fprintf(fil, "  %d;//secondary AI tactic from AI_tactic.dat\n", temp_item.AI_tactics[1]);
		fprintf(fil, "  %d;//footstep particle from particles.dat\n", temp_item.footsteps);
		fprintf(fil, "  %d;//die after how many seconds, 0=doesn't die\n", temp_item.die_after_seconds);
		fprintf(fil, "  %d;//eat item number from items.dat, -1=nothing\n", temp_item.eat_item);
		fprintf(fil, "  %f;//creature size\n", temp_item.size);
		fprintf(fil, "  %f;//creature weight (affects pushing)\n", temp_item.weight);
		fprintf(fil, "  %f;//maximum movement speed\n", temp_item.movement_speed);
		fprintf(fil, "  %f;//minimum movement speed\n", temp_item.minimum_movement_speed);
		fprintf(fil, "  %f;//leg animation speed\n", temp_item.leg_animation_speed);
		fprintf(fil, "  %f;//turn speed\n", temp_item.turn_speed);
		fprintf(fil, "  %f;//inertia level (acceleration/deceleration)\n", temp_item.inertia);
		fprintf(fil, "  %d;//hide when behind walls\n", temp_item.hide_behind_walls);
		fprintf(fil, "  %d;//weapon number\n", temp_item.weapon);
		fprintf(fil, "  %f;//weapon_x\n", temp_item.weapon_x);
		fprintf(fil, "  %f;//weapon_y\n", temp_item.weapon_y);
		fprintf(fil, "  %d;//blood particle\n", temp_item.blood);
		fprintf(fil, "  %d;//bars visible when player hovers mouse on top\n", temp_item.bars_visible);
		fprintf(fil, "  %f;//death animation speed\n", temp_item.death_animation_speed);
		fprintf(fil, "  %d;//creature can move from area to another\n", temp_item.can_change_area);
		fprintf(fil, "  %f;//AI hear range\n", temp_item.AI_hear_range);
		fprintf(fil, "  %f;//AI see range\n", temp_item.AI_see_range);
		fprintf(fil, "  %f;//AI see angle\n", temp_item.AI_see_angle);



		fprintf(fil, "  begin_footstep_sounds\n");
		for(i=0;i<temp_item.footstep_sound_names.size();i++){
			fprintf(fil, "    %s;//sample name\n", temp_item.footstep_sound_names[i].c_str());
		}
		fprintf(fil, "  end_footstep_sounds\n");

		fprintf(fil, "  begin_hit_sounds\n");
		for(i=0;i<temp_item.hit_sound_names.size();i++){
			fprintf(fil, "    %s;//sample name\n", temp_item.hit_sound_names[i].c_str());
		}
		fprintf(fil, "  end_hit_sounds\n");

		fprintf(fil, "  begin_die_sounds\n");
		for(i=0;i<temp_item.die_sound_names.size();i++){
			fprintf(fil, "    %s;//sample name\n", temp_item.die_sound_names[i].c_str());
		}
		fprintf(fil, "  end_die_sounds\n");


		//eat block
		fprintf(fil, "  begin_eat_block\n");
		for(b=0;b<temp_item.eat_block.size();b++){
			fprintf(fil, "    begin_conditions;\n");
			for(c=0;c<temp_item.eat_block[b].conditions.size();c++){
				if(temp_item.eat_block[b].conditions[c].condition_number>=0){
					fprintf(fil, "      %d;//condition number\n", temp_item.eat_block[b].conditions[c].condition_number);
					fprintf(fil, "      %f;//condition parameter0\n", temp_item.eat_block[b].conditions[c].condition_parameter0);
					fprintf(fil, "      %f;//condition parameter1\n", temp_item.eat_block[b].conditions[c].condition_parameter1);
				}
			}
			fprintf(fil, "    end_conditions;\n");

			fprintf(fil, "    begin_effects;\n");
			for(c=0;c<temp_item.eat_block[b].effects.size();c++){
				fprintf(fil, "      %d;//effect number\n", temp_item.eat_block[b].effects[c].effect_number);

				fprintf(fil, "      %f;//parameter1\n", temp_item.eat_block[b].effects[c].parameter1);
				fprintf(fil, "      %f;//parameter2\n", temp_item.eat_block[b].effects[c].parameter2);
				fprintf(fil, "      %f;//parameter3\n", temp_item.eat_block[b].effects[c].parameter3);
				fprintf(fil, "      %f;//parameter4\n", temp_item.eat_block[b].effects[c].parameter4);
			}
			fprintf(fil, "    end_effects;\n");
		}
		fprintf(fil, "  end_eat_block\n");


		//death block
		fprintf(fil, "  begin_death_block\n");
		for(b=0;b<temp_item.death_block.size();b++){
			fprintf(fil, "    %d;//death type, 0=any death, 1=timed death, 2=killed by player\n", temp_item.death_block[b].death_type);

			fprintf(fil, "    begin_conditions;\n");
			for(c=0;c<temp_item.death_block[b].conditions.size();c++){
				if(temp_item.death_block[b].conditions[c].condition_number>=0){
					fprintf(fil, "      %d;//condition number\n", temp_item.death_block[b].conditions[c].condition_number);
					fprintf(fil, "      %f;//condition parameter0\n", temp_item.death_block[b].conditions[c].condition_parameter0);
					fprintf(fil, "      %f;//condition parameter1\n", temp_item.death_block[b].conditions[c].condition_parameter1);
				}
			}
			fprintf(fil, "    end_conditions;\n");

			fprintf(fil, "    begin_effects;\n");
			for(c=0;c<temp_item.death_block[b].effects.size();c++){
				fprintf(fil, "      %d;//effect number\n", temp_item.death_block[b].effects[c].effect_number);

				fprintf(fil, "      %f;//parameter1\n", temp_item.death_block[b].effects[c].parameter1);
				fprintf(fil, "      %f;//parameter2\n", temp_item.death_block[b].effects[c].parameter2);
				fprintf(fil, "      %f;//parameter3\n", temp_item.death_block[b].effects[c].parameter3);
				fprintf(fil, "      %f;//parameter4\n", temp_item.death_block[b].effects[c].parameter4);
			}
			fprintf(fil, "    end_effects;\n");
		}
		fprintf(fil, "  end_death_block\n");


		//hit block
		fprintf(fil, "  begin_hit_block\n");
		for(b=0;b<temp_item.hit_block.size();b++){
			fprintf(fil, "    begin_conditions;\n");
			for(c=0;c<temp_item.hit_block[b].conditions.size();c++){
				if(temp_item.hit_block[b].conditions[c].condition_number>=0){
					fprintf(fil, "      %d;//condition number\n", temp_item.hit_block[b].conditions[c].condition_number);
					fprintf(fil, "      %f;//condition parameter0\n", temp_item.hit_block[b].conditions[c].condition_parameter0);
					fprintf(fil, "      %f;//condition parameter1\n", temp_item.hit_block[b].conditions[c].condition_parameter1);
				}
			}
			fprintf(fil, "    end_conditions;\n");

			fprintf(fil, "    begin_effects;\n");
			for(c=0;c<temp_item.hit_block[b].effects.size();c++){
				fprintf(fil, "      %d;//effect number\n", temp_item.hit_block[b].effects[c].effect_number);

				fprintf(fil, "      %f;//parameter1\n", temp_item.hit_block[b].effects[c].parameter1);
				fprintf(fil, "      %f;//parameter2\n", temp_item.hit_block[b].effects[c].parameter2);
				fprintf(fil, "      %f;//parameter3\n", temp_item.hit_block[b].effects[c].parameter3);
				fprintf(fil, "      %f;//parameter4\n", temp_item.hit_block[b].effects[c].parameter4);
			}
			fprintf(fil, "    end_effects;\n");
		}
		fprintf(fil, "  end_hit_block\n");

		//timed block
		fprintf(fil, "  begin_timed_block\n");
		for(b=0;b<temp_item.timed_block.size();b++){
			fprintf(fil, "    %d;//interval in milliseconds\n", temp_item.timed_block[b].interval);

			fprintf(fil, "    begin_conditions;\n");
			for(c=0;c<temp_item.timed_block[b].conditions.size();c++){
				if(temp_item.timed_block[b].conditions[c].condition_number>=0){
					fprintf(fil, "      %d;//condition number\n", temp_item.timed_block[b].conditions[c].condition_number);
					fprintf(fil, "      %f;//condition parameter0\n", temp_item.timed_block[b].conditions[c].condition_parameter0);
					fprintf(fil, "      %f;//condition parameter1\n", temp_item.timed_block[b].conditions[c].condition_parameter1);
				}
			}
			fprintf(fil, "    end_conditions;\n");

			fprintf(fil, "    begin_effects;\n");
			for(c=0;c<temp_item.timed_block[b].effects.size();c++){
				fprintf(fil, "      %d;//effect number\n", temp_item.timed_block[b].effects[c].effect_number);

				fprintf(fil, "      %f;//parameter1\n", temp_item.timed_block[b].effects[c].parameter1);
				fprintf(fil, "      %f;//parameter2\n", temp_item.timed_block[b].effects[c].parameter2);
				fprintf(fil, "      %f;//parameter3\n", temp_item.timed_block[b].effects[c].parameter3);
				fprintf(fil, "      %f;//parameter4\n", temp_item.timed_block[b].effects[c].parameter4);
			}
			fprintf(fil, "    end_effects;\n");
		}
		fprintf(fil, "  end_timed_block\n");



		fprintf(fil, "  begin_specialties\n");
		for(i=0;i<temp_item.specialties.size();i++){
			fprintf(fil, "  %s;//description\n", temp_item.specialties[i].description.c_str());
			fprintf(fil, "    %d;//number\n", temp_item.specialties[i].number);
			fprintf(fil, "    %f;//parameter0\n", temp_item.specialties[i].parameter0);
			fprintf(fil, "    %f;//parameter1\n", temp_item.specialties[i].parameter1);
			fprintf(fil, "    %f;//parameter2\n", temp_item.specialties[i].parameter2);
			fprintf(fil, "    %f;//parameter3\n", temp_item.specialties[i].parameter3);
		}
		fprintf(fil, "  end_specialties\n");


	}
	fprintf(fil, "end_of_file;\n");
	fprintf(fil, "\n");
	fprintf(fil, "\n");

	fprintf(fil, "specialties:\n");
	fprintf(fil, "0=has bar parameter0 from bars.dat with minimum parameter1 and maximum parameter2 and current value of parameter3 (if any bar reaches minimum, dies)\n");
	fprintf(fil, "1=weapon class parameter0 damage is multiplied by parameter1\n");
	fprintf(fil, "2=carries light parameter0 size parameter1 transparency parameter2, attached to (parameter3, 0=hands, 1=legs, 2=head)\n");
	fprintf(fil, "\n");
	fprintf(fil, "\n");
	fprintf(fil, "Creature Block Templates\n");
	fprintf(fil, "\n");
	fprintf(fil, "[Eat Block]\n");
	fprintf(fil, "  begin_eat_block\n");
	fprintf(fil, "    begin_conditions;\n");
	fprintf(fil, "    end_conditions;\n");
	fprintf(fil, "    begin_effects;\n");
	fprintf(fil, "    end_effects;\n");
	fprintf(fil, "  end_eat_block\n");
	fprintf(fil, "\n");
	fprintf(fil, "[Death Block]\n");
	fprintf(fil, "  begin_death_block\n");
	fprintf(fil, "    0;//death type, 0=any death, 1=timed death, 2=killed by player\n");
	fprintf(fil, "    begin_conditions;\n");
	fprintf(fil, "    end_conditions;\n");
	fprintf(fil, "    begin_effects;\n");
	fprintf(fil, "    end_effects;\n");
	fprintf(fil, "  end_death_block\n");
	fprintf(fil, "\n");
	fprintf(fil, "[Hit Block]\n");
	fprintf(fil, "  begin_hit_block\n");
	fprintf(fil, "    begin_conditions;\n");
	fprintf(fil, "    end_conditions;\n");
	fprintf(fil, "    begin_effects;\n");
	fprintf(fil, "    end_effects;\n");
	fprintf(fil, "  end_hit_block\n");
	fprintf(fil, "\n");
	fprintf(fil, "[Timed Block]\n");
	fprintf(fil, "  begin_timed_block\n");
	fprintf(fil, "    0;//interval in milliseconds\n");
	fprintf(fil, "    begin_conditions;\n");
	fprintf(fil, "    end_conditions;\n");
	fprintf(fil, "    begin_effects;\n");
	fprintf(fil, "    end_effects;\n");
	fprintf(fil, "  end_timed_block\n");
	fprintf(fil, "\n");

	fclose(fil);

  */

}


void Mod::load_weapon_info(const string& filename){//loads object info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	general_weapons.clear();
	string weapon_class_names;

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		general_weapons_base temp_weapon;
		weapon_class_names=stripped_fgets(rivi,sizeof(rivi),fil);
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//object name
			temp_weapon.name=rivi;
			debug->debug_output("Load "+temp_weapon.name,Action::START,Logfile::STARTUP);

			temp_weapon.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_weapon.weapon_class=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			stripped_fgets(rivi,sizeof(rivi),fil);//texture name
			//load texture if it's not loaded
			temp_weapon.texture_name=rivi;
			temp_weapon.texture=resources->load_texture(rivi,mod_name);

			temp_weapon.bullets_at_once=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_weapon.stop_on_hit=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_weapon.size=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//size
			temp_weapon.spread=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//spread
			temp_weapon.bullet_speed=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//speed
			temp_weapon.time=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//time
			temp_weapon.fire_rate=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//fire_rate
			//temp_weapon.energy_consumption=atof(stripped_fgets(rivi,sizeof(rivi),fil));//energy_consumption
			//temp_weapon.use_ammo_from_inventory=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			//temp_weapon.damage=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//damage
			//temp_weapon.gunblast_particle=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_weapon.trace1=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//trace
			/*temp_weapon.special_effect=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//special_effect
			temp_weapon.special_effect_parameter0=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//special_effect_parameter
			temp_weapon.special_effect_parameter1=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//special_effect_parameter
			temp_weapon.special_effect_parameter2=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//special_effect_parameter*/
			temp_weapon.special_effect_visual_color=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//special_effect visual
			temp_weapon.special_effect_visual_particle=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//special_effect visual
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_weapon.special_effect_sound_name=rivi;
			temp_weapon.special_effect_sound=resources->load_sample(rivi,3,mod_name);
			temp_weapon.push=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//push
			temp_weapon.push_shooter=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//push_shooter
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_weapon.sound_fire_name=rivi;
			temp_weapon.sound_fire=resources->load_sample(rivi,3,mod_name);//fire sound
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_weapon.sound_hit_name=rivi;
			temp_weapon.sound_hit=resources->load_sample(rivi,3,mod_name);//hit sound
			temp_weapon.AI_hear_volume=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));

			//wield conditions
			temp_weapon.wield_conditions.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_wield_conditions")!=0){
				Mod::condition temp_condition;

				temp_condition.condition_number=atoi(rivi);
				temp_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

				temp_weapon.wield_conditions.push_back(temp_condition);
			}

			//fire effects
			temp_weapon.fire_effects.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_fire_effects")!=0){
				Mod::effect temp_effect;

				temp_effect.effect_number=atoi(rivi);
				temp_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));


				temp_weapon.fire_effects.push_back(temp_effect);
			}

			//hit effects
			temp_weapon.hit_effects.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_hit_effects")!=0){
				Mod::effect temp_effect;

				temp_effect.effect_number=atoi(rivi);
				temp_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

				temp_weapon.hit_effects.push_back(temp_effect);
			}


			//if the identifier is bigger than current list size, increase list size
			temp_weapon.dead=true;
			while(general_weapons.size()<temp_weapon.identifier+1){
				//put in dummy elements
				general_weapons.push_back(temp_weapon);
			}
			temp_weapon.dead=false;
			general_weapons[temp_weapon.identifier]=temp_weapon;

			debug->debug_output("Load "+temp_weapon.name,Action::END,Logfile::STARTUP);

		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}




void Mod::load_climate_info(const string& filename){//loads climate info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	general_climates.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		general_climate_base temp_climate;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){
			temp_climate.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			string tempstring="Load climate number";
			//tempstring+=itoa(temp_climate.identifier,temprivi,10);
			sprintf(temprivi,"%d",temp_climate.identifier);
			tempstring += temprivi;
			debug->debug_output(tempstring,Action::START,Logfile::STARTUP);

			temp_climate.can_be_random=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));

			//light
			temp_climate.light_oscillate_time=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_phase_r=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_phase_g=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_phase_b=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_amplitude_r=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_amplitude_g=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_amplitude_b=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_max_r=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_max_g=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_max_b=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_min_r=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_min_g=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.light_min_b=atof(stripped_fgets(rivi,sizeof(rivi),fil));

			temp_climate.temperature[0]=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.temperature[1]=atof(stripped_fgets(rivi,sizeof(rivi),fil));

			//weather
			temp_climate.maximum_wind_speed=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.rain_particle=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.rain_particle_life_min=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.rain_particle_life_max=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.rain_probability=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.rain_length_min=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.rain_length_max=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_climate.rain_effects.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects_block")!=0){
				general_climate_base::rain_effect_base temp_effect;

				temp_effect.effect.event_text=rivi;
				temp_effect.interval=atof(stripped_fgets(rivi,sizeof(rivi),fil));

				//conditions
				temp_effect.effect.conditions.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_conditions")!=0){
					Mod::condition temp_effect_condition;

					temp_effect_condition.condition_number=atoi(rivi);
					temp_effect_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effect.conditions.push_back(temp_effect_condition);
				}

				//effects
				temp_effect.effect.effects.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects")!=0){
					Mod::effect temp_effect_effect;

					temp_effect_effect.effect_number=atoi(rivi);
					temp_effect_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effect.effects.push_back(temp_effect_effect);
				}

				temp_climate.rain_effects.push_back(temp_effect);
			}

			//terrains
			temp_climate.terrain_types.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_terrains")!=0){//terrains
				temp_climate.terrain_types.push_back(atoi(rivi));
			}

			//night sounds
			temp_climate.night_sounds.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_night_sounds")!=0){
				temp_climate.night_sounds.push_back(resources->load_sample(rivi,3,mod_name));
			}

			//day sounds
			temp_climate.day_sounds.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_day_sounds")!=0){
				temp_climate.day_sounds.push_back(resources->load_sample(rivi,3,mod_name));
			}

			//props
			temp_climate.prop_amount.clear();
			temp_climate.prop_object_definition_number.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_props")!=0){//props
				temp_climate.prop_object_definition_number.push_back(atoi(rivi));
				temp_climate.prop_amount.push_back(atoi(stripped_fgets(rivi,sizeof(rivi),fil)));
			}


			//if the identifier is bigger than current list size, increase list size
			temp_climate.dead=true;
			while(general_climates.size()<temp_climate.identifier+1){
				//put in dummy elements
				general_climates.push_back(temp_climate);
			}
			temp_climate.dead=false;
			general_climates[temp_climate.identifier]=temp_climate;

			tempstring="Load climate number";
			//tempstring+=itoa(temp_climate.identifier,temprivi,10);
			sprintf(temprivi,"%d",temp_climate.identifier);
			tempstring+=temprivi;
			debug->debug_output(tempstring,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}


void Mod::load_area_info(const string& filename){//loads area info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	general_areas.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//object name
			general_area_special_base temp_area;

			temp_area.identifier=atoi(rivi);
			temp_area.name=stripped_fgets(rivi,sizeof(rivi),fil);
			debug->debug_output("Load "+temp_area.name,Action::START,Logfile::STARTUP);
			temp_area.area_class=atoi(stripped_fgets(rivi,sizeof(rivi),fil));


			temp_area.climate_override=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_area.terrain_map_number=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_area.wrap_type=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_area.random_item_density=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			/*temp_area.near_areas[0]=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_area.near_areas[1]=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_area.near_areas[2]=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_area.near_areas[3]=atoi(stripped_fgets(rivi,sizeof(rivi),fil));*/

			//aliens
			temp_area.alien_amount.clear();
			temp_area.alien_type.clear();
			temp_area.alien_sides.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_alien_list")!=0){
				temp_area.alien_type.push_back(atoi(rivi));
				temp_area.alien_amount.push_back(atoi(stripped_fgets(rivi,sizeof(rivi),fil)));
				temp_area.alien_sides.push_back(atoi(stripped_fgets(rivi,sizeof(rivi),fil)));
			}

			//excluded plot_objects
			temp_area.exclude_plot_objects.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_excluded_plot_object_classes")!=0){
				temp_area.exclude_plot_objects.push_back(atoi(rivi));
			}

			temp_area.on_enter_text=stripped_fgets(rivi,sizeof(rivi),fil);

			temp_area.dead=true;
			//if the identifier is bigger than current list size, increase list size
			while(general_areas.size()<temp_area.identifier+1){
				//put in dummy elements
				general_areas.push_back(temp_area);

			}
			temp_area.dead=false;
			general_areas[temp_area.identifier]=temp_area;

			debug->debug_output("Load "+temp_area.name,Action::END,Logfile::STARTUP);

		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}


void Mod::load_light_info(const string& filename){//loads light info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[800];

	general_lights.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		general_light_base temp_light;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//object name
			temp_light.name=rivi;

			temp_light.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));

			//load texture if it's not loaded
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_light.texture=resources->load_texture(rivi,mod_name);

			temp_light.type=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//type
			temp_light.pulsating=atof(stripped_fgets(rivi,sizeof(rivi),fil));//pulsating
			temp_light.r=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_light.g=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_light.b=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_light.a=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_light.intensity=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_light.particle=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_light.particle_flash_speed=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			//temp_light.particle_size=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_light.particle_time=atof(stripped_fgets(rivi,sizeof(rivi),fil));

			//if the identifier is bigger than current list size, increase list size
			temp_light.dead=true;
			while(general_lights.size()<temp_light.identifier+1){
				//put in dummy elements
				general_lights.push_back(temp_light);
			}
			temp_light.dead=false;
			general_lights[temp_light.identifier]=temp_light;

		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}

void Mod::load_plot_object_info(const string& filename){//loads object info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	general_plot_objects.clear();
	//seen_plot_object_text.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){

		//plot object classes
		plot_object_classes.clear();
		stripped_fgets(rivi,sizeof(rivi),fil);
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_plot_object_classes")!=0){
			plot_object_classes.push_back(rivi);
		}

		//plot objects
		plot_objects_base temp_object;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//object name
			temp_object.name=rivi;
			debug->debug_output("Load "+temp_object.name,Action::START,Logfile::STARTUP);

			temp_object.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.map_type_to_place=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.plot_object_class=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.object_definition_number=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//object_definition_number
			temp_object.amount=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//amount
			temp_object.location_type=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//location_type
			temp_object.location_parameter0=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//location_parameter0
			temp_object.location_parameter1=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//location_parameter1
			temp_object.location_parameter2=(float)atof(stripped_fgets(rivi,sizeof(rivi),fil));//vary_parameter1
			//temp_object.fires=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//fires
			temp_object.show_on_radar=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.show_on_radar_particle=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			//temp_object.clear_area=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//clear_area
			temp_object.sound=-1;
			temp_object.sound_name=stripped_fgets(rivi,sizeof(rivi),fil);
			if(temp_object.sound_name!="none")temp_object.sound=resources->load_sample(rivi,8,mod_name);
			temp_object.live_time=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.trigger_event_by=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//event trigger
			temp_object.trigger_event_parameter1=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_object.show_condition_help=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));

			//uses
			temp_object.effects.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects_block")!=0){
				Mod::effect_base temp_effect;

				temp_effect.event_text=rivi;
				temp_effect.sound=-1;
				temp_effect.sound_name=stripped_fgets(rivi,sizeof(rivi),fil);
				if(temp_effect.sound_name!="none")temp_effect.sound=resources->load_sample(rivi,2,mod_name);//hit sound
				temp_effect.vanish_after_used=atoi(stripped_fgets(rivi,sizeof(rivi),fil));

				//conditions
				temp_effect.conditions.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_conditions")!=0){
					Mod::condition temp_effect_condition;

					temp_effect_condition.condition_number=atoi(rivi);
					temp_effect_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.conditions.push_back(temp_effect_condition);
				}

				//effects
				temp_effect.effects.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects")!=0){
					Mod::effect temp_effect_effect;

					temp_effect_effect.effect_number=atoi(rivi);
					temp_effect_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effects.push_back(temp_effect_effect);
				}

				temp_object.effects.push_back(temp_effect);
			}

			//if the identifier is bigger than current list size, increase list size
			temp_object.dead=true;
			while(general_plot_objects.size()<temp_object.identifier+1){
				//put in dummy elements
				general_plot_objects.push_back(temp_object);
				//seen_plot_object_text.push_back(false);
			}
			temp_object.dead=false;
			general_plot_objects[temp_object.identifier]=temp_object;
			//seen_plot_object_text[temp_object.identifier]=false;

			debug->debug_output("Load "+temp_object.name,Action::END,Logfile::STARTUP);
		}
	}


	fclose(fil);


	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}


void Mod::load_animation_info(const string& filename){//loads object info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	animations.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		animation_base temp_animation;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//object name

			temp_animation.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//identifier

			//load texture
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_animation.texture=resources->load_texture(rivi,mod_name);
			temp_animation.frames=atoi(stripped_fgets(rivi,sizeof(rivi),fil));//frames

			for(int a=0;a<temp_animation.frames;a++){
				stripped_fgets(rivi,sizeof(rivi),fil);//text
				temp_animation.frame[a].text=rivi;
				temp_animation.frame[a].text_y=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_animation.frame[a].start_x=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_animation.frame[a].start_y=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_animation.frame[a].end_x=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_animation.frame[a].end_y=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			}

			//if the identifier is bigger than current list size, increase list size
			while(animations.size()<temp_animation.identifier+1){
				//put in dummy elements
				animations.push_back(temp_animation);
			}
			animations[temp_animation.identifier]=temp_animation;
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}



void Mod::load_race_info(const string& filename){//loads race info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	general_races.clear();
	difficulty_level_descriptions.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		general_race_base temp_race;

		difficulty_level_descriptions.push_back(stripped_fgets(rivi,sizeof(rivi),fil));//easy
		difficulty_level_descriptions.push_back(stripped_fgets(rivi,sizeof(rivi),fil));//medium
		difficulty_level_descriptions.push_back(stripped_fgets(rivi,sizeof(rivi),fil));//hard

		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name

			temp_race.name=rivi;
			debug->debug_output("Load "+temp_race.name,Action::START,Logfile::STARTUP);

			temp_race.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_race.visible_in_start_menu=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_race.side=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_race.start_area=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_race.journal_name=stripped_fgets(rivi,sizeof(rivi),fil);
			temp_race.description=stripped_fgets(rivi,sizeof(rivi),fil);
			temp_race.creature_number=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_race.start_animation=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_race.day_speed=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			//temp_race.death_animation=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			//temp_race.death_by_hunger_animation=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_race.maximum_carry_weight=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			if(temp_race.maximum_carry_weight<=0)temp_race.maximum_carry_weight=0.001f;
			temp_race.temperature_multiplier=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_race.rag_doll=resources->load_texture(rivi,mod_name);
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_race.interface_texture=resources->load_texture(rivi,mod_name);



			//weapon class pictures
			temp_race.weapon_classes.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_weapon_frames")!=0){
				general_race_base::weapon_class temp;

				temp.can_use=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				stripped_fgets(rivi,sizeof(rivi),fil);
				temp.texture=resources->load_texture(rivi,mod_name);
				temp.frame=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp.x=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp.y=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_race.weapon_classes.push_back(temp);
			}


			//specialties
			temp_race.specialties.clear();
			temp_race.slots.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_specialties")!=0){
				specialty temp_special;
				temp_special.number=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				int difficulty=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				switch(difficulty){
					case -2:
						temp_special.difficulty[0]=true;
						temp_special.difficulty[1]=false;
						temp_special.difficulty[2]=false;
						break;
					case -1:
						temp_special.difficulty[0]=true;
						temp_special.difficulty[1]=true;
						temp_special.difficulty[2]=false;
						break;
					case 0:
						temp_special.difficulty[0]=true;
						temp_special.difficulty[1]=true;
						temp_special.difficulty[2]=true;
						break;
					case 1:
						temp_special.difficulty[0]=false;
						temp_special.difficulty[1]=true;
						temp_special.difficulty[2]=true;
						break;
					case 2:
						temp_special.difficulty[0]=false;
						temp_special.difficulty[1]=false;
						temp_special.difficulty[2]=true;
						break;
				}

				temp_special.message=stripped_fgets(rivi,sizeof(rivi),fil);
				temp_special.parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_special.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_special.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_special.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_race.specialties.push_back(temp_special);

				//it's a slot specialty
				if(temp_special.number==2){
					general_race_base::slot_base temp_slot;
					strcpy(rivi,temp_special.message.c_str());
					temp_slot.texture=resources->load_texture(rivi,mod_name);
					temp_slot.x=temp_special.parameter1;
					temp_slot.y=temp_special.parameter2;

					temp_slot.active=false;
					while(temp_race.slots.size()<temp_special.parameter0+1){
						temp_race.slots.push_back(temp_slot);
						//slot_used_by.push_back(-1);
					}
					temp_slot.active=true;
					temp_race.slots[temp_special.parameter0]=temp_slot;
				}
			}

			//disabled endings
			temp_race.disabled_endings.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_disabled_endings")!=0){
				temp_race.disabled_endings.push_back(atoi(rivi));
			}

			//disabled item classes
			temp_race.disabled_item_classes.clear();
			temp_race.disabled_item_classes_text.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_disabled_item_classes")!=0){
				temp_race.disabled_item_classes.push_back(atoi(rivi));
				temp_race.disabled_item_classes_text.push_back(stripped_fgets(rivi,sizeof(rivi),fil));
			}

			temp_race.dead=true;
			//if the identifier is bigger than current list size, increase list size
			while(general_races.size()<temp_race.identifier+1){
				//put in dummy elements
				general_races.push_back(temp_race);

			}
			temp_race.dead=false;
			general_races[temp_race.identifier]=temp_race;

			debug->debug_output("Load "+temp_race.name,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}

void Mod::load_polygons(const string& filename){//loads polygon info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	polygons.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		polygon_base temp_polygon;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name
			temp_polygon.name=rivi;
			debug->debug_output("Load "+temp_polygon.name,Action::START,Logfile::STARTUP);
			temp_polygon.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));

			//points
			temp_polygon.points.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_points")!=0){
				point2d temp_point;

				temp_point.x=atof(rivi);
				temp_point.y=atof(stripped_fgets(rivi,sizeof(rivi),fil));

				temp_polygon.points.push_back(temp_point);
			}

			grow_polygon(&temp_polygon);

			while(polygons.size()<temp_polygon.identifier+1){
				//put in dummy elements
				polygons.push_back(temp_polygon);
			}
			polygons[temp_polygon.identifier]=temp_polygon;

			debug->debug_output("Load "+temp_polygon.name,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}

void Mod::grow_polygon(polygon_base *temp_polygon){
	int a;
	temp_polygon->grown_points.clear();
	for(a=0;a<temp_polygon->points.size()-1;a++){

		//find the right direction from next point
		float direction_x=temp_polygon->points[a+1].x-temp_polygon->points[a].x;
		float direction_y=temp_polygon->points[a+1].y-temp_polygon->points[a].y;
		float length_1=sqrt(sqr(direction_x)+sqr(direction_y));
		direction_x=direction_x/length_1;
		direction_y=direction_y/length_1;

		//find the right direction from previous point
		int previous=a-1;
		if(previous<0)previous=temp_polygon->points.size()-2;
		float direction_x_2=temp_polygon->points[previous].x-temp_polygon->points[a].x;
		float direction_y_2=temp_polygon->points[previous].y-temp_polygon->points[a].y;
		float length_2=sqrt(sqr(direction_x_2)+sqr(direction_y_2));
		direction_x_2=direction_x_2/length_2;
		direction_y_2=direction_y_2/length_2;

		point2d temp_point;
		temp_point.x=temp_polygon->points[a].x-direction_x*0.001f-direction_x_2*0.001f;
		temp_point.y=temp_polygon->points[a].y-direction_y*0.001f-direction_y_2*0.001f;

		temp_polygon->grown_points.push_back(temp_point);
	}

	//finally, the first point is also the last
	temp_polygon->grown_points.push_back(temp_polygon->grown_points[0]);

}



void Mod::load_scripts(const string& filename){
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	scripts.clear();
	//scripts_calculated_on.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		script temp_script;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//object name
			temp_script.name=rivi;
			debug->debug_output("Load "+temp_script.name,Action::START,Logfile::STARTUP);

			temp_script.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_script.run_without_calling=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_script.calling_position=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_script.calling_creature=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_script.interval=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_script.message=stripped_fgets(rivi,sizeof(rivi),fil);
			temp_script.message_type=atoi(stripped_fgets(rivi,sizeof(rivi),fil));

			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_script.sound_name=rivi;
			temp_script.sound=resources->load_sample(rivi,3,mod_name);
			temp_script.delay=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			//temp_script.timer=0;
			temp_script.disable_after_first_use=atoi(stripped_fgets(rivi,sizeof(rivi),fil));

			//conditions
			temp_script.conditions.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_conditions")!=0){
				Mod::condition temp_condition;

				temp_condition.condition_number=atoi(rivi);
				temp_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

				temp_script.conditions.push_back(temp_condition);
			}

			//effects
			temp_script.effects.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects")!=0){
				Mod::effect temp_effect;

				temp_effect.effect_number=atoi(rivi);
				temp_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

				temp_script.effects.push_back(temp_effect);
			}


			//if the identifier is bigger than current list size, increase list size
			temp_script.dead=true;
			while(scripts.size()<temp_script.identifier+1){
				//put in dummy elements
				scripts.push_back(temp_script);
				//scripts_calculated_on.push_back(0);
			}
			temp_script.dead=false;
			scripts[temp_script.identifier]=temp_script;
			//scripts_calculated_on[temp_script.identifier]=randDouble(0,temp_script.interval*0.001f);

			debug->debug_output("Load "+temp_script.name,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);

}

void Mod::load_AI_side(const string& filename){//loads AI info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	AI_sides.clear();

	//first go throught the file and find how many sides there are
	fil = fopen(filename.c_str(),"rt");
	if(fil){

		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name

			AI_side temp_side;
			temp_side.friend_with_side.clear();
			AI_sides.push_back(temp_side);

			stripped_fgets(rivi,sizeof(rivi),fil);
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_hostile_sides")!=0){

			}

		}
	}
	fclose(fil);


	//for all current sides add one friendly side
	for(int a=0;a<AI_sides.size();a++){
		for(int b=0;b<AI_sides.size();b++){
			AI_sides[a].friend_with_side.push_back(true);
		}

	}

	//then go throught the file and add all enemy sides
	fil = fopen(filename.c_str(),"rt");
	int side=0;
	if(fil){

		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name

			AI_sides[side].name=rivi;

			stripped_fgets(rivi,sizeof(rivi),fil);
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_hostile_sides")!=0){
				AI_sides[side].friend_with_side[atoi(rivi)]=false;
			}
			side++;
		}
	}
	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}


void Mod::load_bars(const string& filename){//loads bar info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];


	general_bars.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		bar_base temp_bar;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name
			temp_bar.name=rivi;

			debug->debug_output("Load "+temp_bar.name,Action::START,Logfile::STARTUP);

			temp_bar.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.bar_type=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.visible=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.visible_on_enemies=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.show_number=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.anchor_point=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.location_x=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.location_y=atof(stripped_fgets(rivi,sizeof(rivi),fil));


			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_bar.bar_picture=resources->load_texture(rivi,mod_name);
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_bar.background_picture=resources->load_texture(rivi,mod_name);
			temp_bar.background_picture_x_offset=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.background_picture_y_offset=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.background_picture_width=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.background_picture_height=atof(stripped_fgets(rivi,sizeof(rivi),fil));

			temp_bar.height=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.length=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.color_min_r=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.color_min_g=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.color_min_b=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.color_max_r=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.color_max_g=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_bar.color_max_b=atof(stripped_fgets(rivi,sizeof(rivi),fil));

			temp_bar.dead=true;
			while(general_bars.size()<temp_bar.identifier+1){
				//put in dummy elements
				general_bars.push_back(temp_bar);
			}
			temp_bar.dead=false;
			general_bars[temp_bar.identifier]=temp_bar;


			debug->debug_output("Load "+temp_bar.name,Action::END,Logfile::STARTUP);

		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}




void Mod::load_AI_info(const string& filename){//loads AI info from file
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	AI_tactics.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		AI_tactic_base temp_tactic;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name
			temp_tactic.name=rivi;
			debug->debug_output("Load "+temp_tactic.name,Action::START,Logfile::STARTUP);

			temp_tactic.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));

			float start=0;
			temp_tactic.levels.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_levels")!=0){
				AI_tactic_base::anger_level temp_level;
				temp_level.size=atof(rivi);
				temp_level.start=start;
				start+=temp_level.size;
				temp_level.action=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_level.parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_level.can_shoot=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_level.can_eat=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));

				temp_tactic.levels.push_back(temp_level);
			}

			//normalize levels
			float multiplier=1/start;
			for(int a=0;a<temp_tactic.levels.size();a++){
				temp_tactic.levels[a].start=temp_tactic.levels[a].start*multiplier;
				temp_tactic.levels[a].size=temp_tactic.levels[a].size*multiplier;
			}

			AI_tactics.push_back(temp_tactic);

			debug->debug_output("Load "+temp_tactic.name,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}


void Mod::load_terrain_types(const string& filename){
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	terrain_types.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		terrain_type_base temp_terrain;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name
			temp_terrain.name=rivi;
			debug->debug_output("Load "+temp_terrain.name,Action::START,Logfile::STARTUP);

			temp_terrain.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_terrain.AI_avoid=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_terrain.do_not_place_on_map_edges=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_terrain.do_not_place_random_objects=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_terrain.base_r=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_terrain.base_g=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_terrain.base_b=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_terrain.footstep_particle_time=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_terrain.override_footstep_particle=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			stripped_fgets(rivi,sizeof(rivi),fil);
			temp_terrain.override_footstep_sound=resources->load_sample(rivi,5,mod_name);
			temp_terrain.override_footstep_sound_name=rivi;

			//terrain frames
			temp_terrain.terrain_frames.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_terrain_frames")!=0){
				object_animation_frame_base temp_frame;

				temp_frame.texture=resources->load_texture(rivi,mod_name);
				temp_frame.texture_name=rivi;
				temp_frame.time=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_terrain.terrain_frames.push_back(temp_frame);
			}

			//effects
			temp_terrain.effects.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects_block")!=0){
				terrain_type_base::terrain_effect_base temp_effect;

				temp_effect.effect.event_text=rivi;
				temp_effect.interval=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_effect.effect.vanish_after_used=atoi(stripped_fgets(rivi,sizeof(rivi),fil));

				//conditions
				temp_effect.effect.conditions.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_conditions")!=0){
					Mod::condition temp_effect_condition;

					temp_effect_condition.condition_number=atoi(rivi);
					temp_effect_condition.condition_parameter0=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_condition.condition_parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effect.conditions.push_back(temp_effect_condition);
				}

				//effects
				temp_effect.effect.effects.clear();
				stripped_fgets(rivi,sizeof(rivi),fil);
				while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_effects")!=0){
					Mod::effect temp_effect_effect;

					temp_effect_effect.effect_number=atoi(rivi);
					temp_effect_effect.parameter1=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter2=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter3=atof(stripped_fgets(rivi,sizeof(rivi),fil));
					temp_effect_effect.parameter4=atof(stripped_fgets(rivi,sizeof(rivi),fil));

					temp_effect.effect.effects.push_back(temp_effect_effect);
				}

				temp_terrain.effects.push_back(temp_effect);
			}


			//if the identifier is bigger than current list size, increase list size
			temp_terrain.dead=true;
			while(terrain_types.size()<temp_terrain.identifier+1){
				//put in dummy elements
				terrain_types.push_back(temp_terrain);
				//seen_plot_object_text.push_back(false);
			}
			temp_terrain.dead=false;
			terrain_types[temp_terrain.identifier]=temp_terrain;


			//terrain_types.push_back(temp_terrain);

			debug->debug_output("Load "+temp_terrain.name,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}



void Mod::load_terrain_maps(const string& filename){
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	terrain_maps.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		terrain_map_base temp_map;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name
			temp_map.name=rivi;
			debug->debug_output("Load "+temp_map.name,Action::START,Logfile::STARTUP);

			temp_map.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			/* int x = */ atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			/* int y = */ atoi(stripped_fgets(rivi,sizeof(rivi),fil));

			temp_map.terrain_grid.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_map_grid")!=0){

				//separate by ,
				terrain_map_base::terrain_grid_row_base temp_row;

				//fgets(rivi,sizeof(rivi),fil);

				vector<string>  ls;
				stringtok (ls, rivi, ",");
				//int d=ls.size();
				//separate by .
				for(int b=0;b<ls.size();b++){

					terrain_map_base::terrain_grid_base	temp_point;
					vector<string> terrain_point_parts;
					stringtok (terrain_point_parts, ls[b].c_str(), ".");

					temp_point.terrain_type=atoi(terrain_point_parts[0].c_str());
					temp_point.no_random_items=atoi(terrain_point_parts[1].c_str());

					temp_row.terrain_blocks.push_back(temp_point);
					//map_list[a*height+b]=atoi(ls[b].c_str());
				}
				temp_map.terrain_grid.push_back(temp_row);
			}

			temp_map.map_objects.clear();
			stripped_fgets(rivi,sizeof(rivi),fil);
			while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_objects")!=0){
				terrain_map_base::editor_object_base temp_object;

				temp_object.type=atoi(rivi);
				temp_object.number=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_object.x=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_object.y=atof(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_object.rotation=strtod(stripped_fgets(rivi,sizeof(rivi),fil),NULL);
				temp_object.amount=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_object.side=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
				temp_object.size=atof(stripped_fgets(rivi,sizeof(rivi),fil));

				temp_map.map_objects.push_back(temp_object);
			}

			//if the identifier is bigger than current list size, increase list size
			temp_map.dead=true;
			while(terrain_maps.size()<temp_map.identifier+1){
				//put in dummy elements
				terrain_maps.push_back(temp_map);
			}
			temp_map.dead=false;
			terrain_maps[temp_map.identifier]=temp_map;


			debug->debug_output("Load "+temp_map.name,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}


void Mod::save_terrain_maps(string filename){

	if(filename=="")filename="data/"+mod_name+"/terrain_maps.dat";

	debug->debug_output("Save file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;

	fil = fopen(filename.c_str(),"wt");
	terrain_map_base temp_map;
	for(unsigned int a=0;a<terrain_maps.size();a++){
		temp_map=terrain_maps[a];
		if(temp_map.dead)continue;

		fprintf(fil, "%s;//name---------------------------------------\n", temp_map.name.c_str());
		fprintf(fil, "  %d;//identifier\n", temp_map.identifier);
		fprintf(fil, "  %lu;//width\n", temp_map.terrain_grid[0].terrain_blocks.size());
		fprintf(fil, "  %lu;//height\n", temp_map.terrain_grid.size());

		//terrains
		fprintf(fil, "  begin_map_grid;\n");
		for(unsigned int b=0;b<temp_map.terrain_grid.size();b++){
			fprintf(fil, "    ");
			for(unsigned int c=0;c<temp_map.terrain_grid[b].terrain_blocks.size();c++){
				fprintf(fil, "%d", temp_map.terrain_grid[b].terrain_blocks[c].terrain_type);
				fprintf(fil, ".");
				fprintf(fil, "%d", temp_map.terrain_grid[b].terrain_blocks[c].no_random_items);
				if(c==temp_map.terrain_grid[b].terrain_blocks.size()-1)
					fprintf(fil, "\n");
				else
					fprintf(fil, ",");
			}
		}
		fprintf(fil, "  end_map_grid;\n");

		//objects
		fprintf(fil, "  begin_objects;\n");
		for(unsigned int b=0;b<temp_map.map_objects.size();b++){
			int x_grid=temp_map.map_objects[b].x/128;
			int y_grid=temp_map.map_objects[b].y/128;
			//is outside borders
			if((y_grid>=temp_map.terrain_grid.size())||(x_grid>=temp_map.terrain_grid[y_grid].terrain_blocks.size())){
				continue;
			}

			//float a=temp_map.map_objects[b].rotation;

			fprintf(fil, "  %d;//type\n", temp_map.map_objects[b].type);
			fprintf(fil, "  %d;//number\n", temp_map.map_objects[b].number);
			fprintf(fil, "  %f;//x\n", temp_map.map_objects[b].x);
			fprintf(fil, "  %f;//y\n", temp_map.map_objects[b].y);
			fprintf(fil, "  %.10f;//angle\n", temp_map.map_objects[b].rotation);
			fprintf(fil, "  %d;//amount\n", temp_map.map_objects[b].amount);
			fprintf(fil, "  %d;//side\n", temp_map.map_objects[b].side);
			fprintf(fil, "  %f;//size\n", temp_map.map_objects[b].size);

		}
		fprintf(fil, "  end_objects;\n");

	}
	fprintf(fil, "end_of_file;\n");
	fprintf(fil, "\n");
	fprintf(fil, "\n");
	fprintf(fil, "This file is meant to be edited with the map editor.\n");
	fprintf(fil, "Start the game in debug mode to see the button in the new game menu.\n");
	fprintf(fil, "Activate the debug mode by setting the first number after the '-' to 1 in setup.dat.\n");

	fclose(fil);

	debug->debug_output("Save file "+filename,Action::END,Logfile::STARTUP);
}


void Mod::load_dialogs(const string& filename){
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	dialogs.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		dialog_base temp_dialog;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name
			temp_dialog.name=rivi;
			debug->debug_output("Load "+temp_dialog.name,Action::START,Logfile::STARTUP);

			temp_dialog.identifier=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_dialog.text=stripped_fgets(rivi,sizeof(rivi),fil);
			temp_dialog.duration=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_dialog.next_line=atoi(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_dialog.r=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_dialog.g=atof(stripped_fgets(rivi,sizeof(rivi),fil));
			temp_dialog.b=atof(stripped_fgets(rivi,sizeof(rivi),fil));

			//if the identifier is bigger than current list size, increase list size
			temp_dialog.dead=true;
			while(dialogs.size()<temp_dialog.identifier+1){
				//put in dummy elements
				dialogs.push_back(temp_dialog);
			}
			temp_dialog.dead=false;
			dialogs[temp_dialog.identifier]=temp_dialog;

			debug->debug_output("Load "+temp_dialog.name,Action::END,Logfile::STARTUP);
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}


void Mod::load_music(const string& filename){
	debug->debug_output("Load file "+filename,Action::START,Logfile::STARTUP);

	FILE *fil;
	char rivi[2000];

	music.clear();

	fil = fopen(filename.c_str(),"rt");
	if(fil){
		music_base temp_music;
		while(strcmp(stripped_fgets(rivi,sizeof(rivi),fil),"end_of_file")!=0){//name

			temp_music.identifier=atoi(rivi);
			temp_music.name=stripped_fgets(rivi,sizeof(rivi),fil);
			temp_music.can_be_random=strtobool(stripped_fgets(rivi,sizeof(rivi),fil));

			//if the identifier is bigger than current list size, increase list size
			temp_music.dead=true;
			while(music.size()<temp_music.identifier+1){
				//put in dummy elements
				music.push_back(temp_music);
			}
			temp_music.dead=false;
			music[temp_music.identifier]=temp_music;
		}
	}

	fclose(fil);

	debug->debug_output("Load file "+filename,Action::END,Logfile::STARTUP);
}

void Mod::load_mod(const string& mod, debugger *debugger, resource_handler *resources){
	debug=debugger;
	this->resources=resources;
	this->mod_name=mod;

	load_object_info("data/"+mod_name+"/object_definitions.dat");
	load_climate_info("data/"+mod_name+"/climate_types.dat");
	load_creature_info("data/"+mod_name+"/creatures.dat");
	load_light_info("data/"+mod_name+"/light.dat");
	load_weapon_info("data/"+mod_name+"/weapons.dat");
	load_item_info("data/"+mod+"/items.dat");
	load_area_info("data/"+mod_name+"/areas.dat");
	load_plot_object_info("data/"+mod_name+"/plot_objects.dat");
	load_animation_info("data/"+mod_name+"/animation.dat");
	load_race_info("data/"+mod_name+"/player_races.dat");
	load_AI_info("data/"+mod_name+"/AI_tactic.dat");
	load_AI_side("data/"+mod_name+"/sides.dat");
	load_bars("data/"+mod_name+"/bars.dat");
	load_scripts("data/"+mod_name+"/scripts.dat");
	load_polygons("data/"+mod_name+"/polygons.dat");
	load_terrain_types("data/"+mod_name+"/terrain_types.dat");
	load_terrain_maps("data/"+mod_name+"/terrain_maps.dat");
	load_dialogs("data/"+mod_name+"/dialogs.dat");
	load_music("data/"+mod_name+"/music.dat");

	//save_terrain_maps("data/"+mod_name+"/terrain_maps.dat");
}
