# pragma once

#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "namespace_attributes.h"
#include "itemkeys_to_itemnames.h"

//#include "Lynnette_Twitter_native.h"
#include "Lynnette_Twitter_Probabilities.h"
//#include "Lynnette_twitter.h"

using namespace attributes;

int main() {
	try {
	// Simulation specific values
	unsigned int time_count = 2;
	unsigned int knowledge_count = 5; //required regardless
	float deltat = 1.0f; // in hours
	bool ora_nodeset = true; // switch this to true to load in your ora_nodeset

	// Construct initialization
	Construct construct;
	construct.verbose_runtime = true;
	construct.random.set_seed(time(nullptr));
	construct.time_count = time_count;

	dynet::ParameterMap generator_params;

	// Initialize nodesets
	Nodeset* knowledge_ns = construct.ns_manager.create_nodeset(nodeset_names::knowledge); 	//knowledge required regardless
	knowledge_ns->add_nodes(knowledge_count, dynet::ParameterMap());

	Nodeset* time_ns = construct.ns_manager.create_nodeset(nodeset_names::time);
	time_ns->add_nodes(time_count, dynet::ParameterMap());

	Nodeset* agent_ns = construct.ns_manager.create_nodeset(nodeset_names::agents);
	Nodeset* attributes_ns = construct.ns_manager.create_nodeset(attributes::nodeset_graph_attributes);

	agent_ns->import_dynetml(attributes::ora_file_input, attributes::nodeset_agent);
	attributes_ns->import_dynetml(attributes::ora_file_input, attributes::nodeset_attributes);

	// Initialize graphs

	Graph<bool>* knowledge_net = construct.graph_manager.load_optional(graph_names::knowledge, true, agent_ns, dense, knowledge_ns, sparse); 	// default value of knowledge net = true means you know the particular piece of knowledge (or know about the hashtag)


	Graph<bool>* follower_network = construct.graph_manager.load_optional(graph_names::twit_follow, false, agent_ns, dense, agent_ns, sparse);
	generator_params.clear();
	generator_params["network name"] = attributes::network_all_communication;
	generator_params["file"] = ora_file_input;
	construct.graph_manager.generators.dynetml_generator(generator_params, follower_network);

	// Load all Agent x Attributes network
	Graph<float>* retweet_probability_network = construct.graph_manager.load_optional(attributes::graph_agent_attributes, 0.0f, agent_ns, dense, attributes_ns, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_agent_attributes_retweets;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, retweet_probability_network);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	Graph<float>* replies_probability_network = construct.graph_manager.load_optional(attributes::graph_agent_attributes, 0.0f, agent_ns, dense, attributes_ns, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_agent_attributes_replies;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, replies_probability_network);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	Graph<float>* quotes_probability_network = construct.graph_manager.load_optional(attributes::graph_agent_attributes, 0.0f, agent_ns, dense, attributes_ns, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_agent_attributes_quotes;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, quotes_probability_network);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	Graph<float>* likes_probability_network = construct.graph_manager.load_optional(attributes::graph_agent_attributes, 0.0f, agent_ns, dense, attributes_ns, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_agent_attributes_likes;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, likes_probability_network);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	Graph<float>* bende_probability_network = construct.graph_manager.load_optional(attributes::graph_agent_attributes, 0.0f, agent_ns, dense, attributes_ns, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_agent_attributes_probabilities;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, bende_probability_network);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	//put all output networks
	Graph<unsigned int>* retweet_output_network = construct.graph_manager.load_optional(attributes::retweet_output_network, 0u, nodeset_names::agents , dense, nodeset_names::time, dense);

	Graph<unsigned int>* replies_output_network = construct.graph_manager.load_optional(attributes::replies_output_network, 0u, nodeset_names::agents, dense, nodeset_names::time, dense);

	Graph<unsigned int>* quotes_output_network = construct.graph_manager.load_optional(attributes::quotes_output_network, 0u, nodeset_names::agents, dense, nodeset_names::time, dense);

	Graph<unsigned int>* likes_output_network = construct.graph_manager.load_optional(attributes::likes_output_network, 0u, nodeset_names::agents, dense, nodeset_names::time, dense);

	//Add item names 
	addInteractionItemNames();

	// Adding the twitter model, additional models can be added here as well
	// Add models after loading all the nodesets and networks
	dynet::ParameterMap model_params;
	model_params["interval time duration"] = std::to_string(deltat);
	model_params["maximum post inactivity"] = "24";

	Model* model;
	try {
		model = new Lynnette_Twitter(model_params, construct);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	construct.model_manager.add_model(model_names::TWIT_wf, model);

	//// Add outputs only after all models have been loaded
	dynet::ParameterMap output_params;

	output_params["model name"] = model_names::TWIT_wf;
	output_params["start time"] = "2024-03-12T02:26:00.000Z";
	output_params["time conversion to seconds"] = "3600";
	output_params["output file"] = attributes::output_json;

	Output* out = new Output_Events(output_params, construct);
	construct.output_manager.add_output(out);

	output_params.clear();
	output_params["network names"] = attributes::retweet_output_network + "," + 
		attributes::replies_output_network + "," +
		attributes::quotes_output_network + "," +
		attributes::likes_output_network + "," +
		graph_names::twit_follow;
	output_params["time periods"] = "all";
	output_params["output file"] = attributes::output_dyxml;
	out = new Output_dynetml(output_params, construct);
	construct.output_manager.add_output(out);

	construct.run();
	}
	catch (dynet::construct_exception e) {
		std::cout << e.what();
	}

}