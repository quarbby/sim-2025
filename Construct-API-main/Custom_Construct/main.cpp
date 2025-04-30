# pragma once

#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "namespace_attributes.h"

#include "main_helper.h"

#include "Lynnette_twitter.h"
#include "Twitter_Formulation2.h"
#include "Twitter_Formulation3.h"

using namespace attributes;

#define TWITTERFORMULATION 2
#define TIME_COUNT 5
#define DELTA_T 1.0	//in hours
//#define USE_ORA_NODESET true

int main() {
	try {
	// Construct initialization
	Construct construct;
	construct.verbose_runtime = true;
	construct.random.set_seed(time(nullptr));
	construct.time_count = TIME_COUNT;

	dynet::ParameterMap generator_params;

	// Initialize nodesets
	Nodeset* agentNodeset = initializeAgentNodeset(construct);
	Nodeset* attributesNodeset = initializeAttributesNodeset(construct);
	Nodeset* knowledgeNodeset = initializeKnowledgeNodeset(construct);
	Nodeset* timeNodeset = initializeTimeNodeset(construct);

	// Initialize graphs
	createKnowledgeNet(construct, agentNodeset, knowledgeNodeset);

	Graph<bool>* follower_network = construct.graph_manager.load_optional(graph_names::twit_follow, false, agentNodeset, dense, agentNodeset, sparse);
	generator_params.clear();
	generator_params["network name"] = attributes::network_all_communication;
	generator_params["file"] = ora_file_input;
	construct.graph_manager.generators.dynetml_generator(generator_params, follower_network);

	// Load all Agent x Attributes networks
	Graph<float>* retweet_probability_network = construct.graph_manager.load_optional(attributes::graph_retweets, 0.0f, agentNodeset, dense, attributesNodeset, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_input_retweets;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, retweet_probability_network);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	Graph<float>* replies_probability_network = construct.graph_manager.load_optional(attributes::graph_replies, 0.0f, agentNodeset, dense, attributesNodeset, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_input_replies;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, replies_probability_network);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	Graph<float>* quotes_probability_network = construct.graph_manager.load_optional(attributes::graph_quotes, 0.0f, agentNodeset, dense, attributesNodeset, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_input_quotes;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, quotes_probability_network);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	Graph<float>* likes_probability_network = construct.graph_manager.load_optional(attributes::graph_likes, 0.0f, agentNodeset, dense, attributesNodeset, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_input_likes;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, likes_probability_network);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	Graph<float>* bende_probability_network = construct.graph_manager.load_optional(attributes::graph_bende_probabilities, 0.0f, agentNodeset, dense, attributesNodeset, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_input_probabilities;
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
	model_params["interval time duration"] = std::to_string(DELTA_T);
	model_params["maximum post inactivity"] = "24";

	Model* model;
	try {
		if (TWITTERFORMULATION == 1)
		{
			Lynnette_Twitter* lynnetteTwitter = new Lynnette_Twitter(model_params, construct);
			lynnetteTwitter->setupNetwork();
			model = (Model*)lynnetteTwitter;
		}
		else if (TWITTERFORMULATION == 2)
		{
			TwitterFormulation2* lynnetteTwitter = new TwitterFormulation2(model_params, construct);
			lynnetteTwitter->setupNetwork();
			model = (Model*)lynnetteTwitter;
		}
		else if (TWITTERFORMULATION == 3)
		{
			TwitterFormulation3* lynnetteTwitter = new TwitterFormulation3(model_params, construct);
			lynnetteTwitter->setupNetwork();
			model = (Model*)lynnetteTwitter;
		}
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}

	construct.model_manager.add_model(model_names::TWIT_wf, model);

	initializeOutputs(construct);

	construct.run();
	}
	catch (dynet::construct_exception e) {
		std::cout << e.what();
	}

}