#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include <cmath>
#include <format>

#include "item_keys.h"
#include "attributes.h"
#include "bendEmotions.h"

#include "MainHelper.h"
#include "OutputTwitterPosts.h"

using namespace attributes;

const int KNOWLEDGE_COUNT = 5;

Nodeset* initializeKnowledgeNodeset(Construct& construct)
{
	Nodeset* knowledgeNodeset = construct.ns_manager.create_nodeset(nodeset_names::knowledge); 	//knowledge required regardless
	knowledgeNodeset->add_nodes(KNOWLEDGE_COUNT, dynet::ParameterMap());

	return knowledgeNodeset;

}

Nodeset* initializeTimeNodeset(Construct& construct)
{
	Nodeset* timeNodeset = construct.ns_manager.create_nodeset(nodeset_names::time);
	timeNodeset->add_nodes(construct.time_count, dynet::ParameterMap());

	return timeNodeset;

}

Nodeset* initializeAgentNodeset(Construct& construct)
{
	Nodeset* agentNodeset = construct.ns_manager.create_nodeset(nodeset_names::agents);
	agentNodeset->import_dynetml(attributes::ora_file_input, attributes::nodeset_agent);

	return agentNodeset;

}

Nodeset* initializeAttributesNodeset(Construct& construct)
{
	Nodeset* attributesNodeset = construct.ns_manager.create_nodeset(attributes::nodeset_graph_attributes);
	attributesNodeset->import_dynetml(attributes::ora_file_input, attributes::nodeset_graph_attributes);

	return attributesNodeset;

}

Nodeset* initializeEngagementNodeset(Construct& construct)
{
	Nodeset* engagementNodeset = construct.ns_manager.create_nodeset(attributes::nodeset_graph_engagement);
	engagementNodeset->import_dynetml(attributes::ora_file_input, attributes::nodeset_graph_engagement);

	return engagementNodeset;
}

void createKnowledgeNet(Construct& construct, Nodeset* agentNodeset, Nodeset* knowledgeNodeset)
{
	// default value of knowledge net = true means you know the particular piece of knowledge (or know about the hashtag)
	Graph<bool>* knowledgeNetwork = construct.graph_manager.load_optional(graph_names::knowledge, true, agentNodeset, dense, knowledgeNodeset, sparse); 	
}

void initializeOutputs(Construct& construct)
{
	dynet::ParameterMap outputParams;

	std::chrono::system_clock::time_point currentTime = std::chrono::system_clock::now();
	std::string datetimeString = std::format("_{:%Y-%m-%d_%H%M}", currentTime);

	outputParams.clear();
	outputParams["network names"] = attributes::retweet_output_network + "," +
		attributes::replies_output_network + "," +
		attributes::quotes_output_network + "," +
		//attributes::likes_output_network + "," +
		graph_names::twit_follow;
	outputParams["time periods"] = "all";
	outputParams["output file"] = attributes::output_dyxml + datetimeString + ".xml";
	
	Output* out = new Output_dynetml(outputParams, construct);
	construct.output_manager.add_output(out);

	outputParams.clear();
	outputParams["output file"] = attributes::output_posts_json + datetimeString + ".csv";
	outputParams["frequency"] = attributes::output_posts_frequency;
	OutputTwitterPosts* m_OutputTwitterPosts = new OutputTwitterPosts(outputParams, construct);
	construct.output_manager.add_output(m_OutputTwitterPosts);
}

void addInteractionItemNames() {
	InteractionItem::item_names[InteractionItem::item_keys::Lynnette_start] = bendEmotions::itemkey_lynnette_start;
	InteractionItem::item_names[InteractionItem::item_keys::likes] = bendEmotions::itemkey_likes;
	InteractionItem::item_names[InteractionItem::item_keys::retweets] = bendEmotions::itemkey_retweets;
	InteractionItem::item_names[InteractionItem::item_keys::quotes] = bendEmotions::itemkey_quotes;
	InteractionItem::item_names[InteractionItem::item_keys::reply] = bendEmotions::itemkey_reply;
	InteractionItem::item_names[InteractionItem::item_keys::bendE] = bendEmotions::itemkey_bendE;
	InteractionItem::item_names[InteractionItem::item_keys::Lynnette_end] = bendEmotions::itemkey_lynnette_end;

	InteractionItem::item_names[InteractionItem::item_keys::BendE_start] = bendEmotions::itemkey_bende_start;
	InteractionItem::item_names[InteractionItem::item_keys::will_retweet] = bendEmotions::itemkey_willRetweet;
	InteractionItem::item_names[InteractionItem::item_keys::will_reply] = bendEmotions::itemkey_willReply;
	InteractionItem::item_names[InteractionItem::item_keys::will_quote] = bendEmotions::itemkey_willQuote;
	InteractionItem::item_names[InteractionItem::item_keys::BendE_end] = bendEmotions::itemkey_bende_end;
}

void createOutputNetworks(Construct& construct)
{
	Graph<unsigned int>* retweetOutputNetwork = construct.graph_manager.load_optional(attributes::retweet_output_network, 0u, nodeset_names::agents, dense, nodeset_names::time, dense);

	Graph<unsigned int>* repliesOutputNetwork = construct.graph_manager.load_optional(attributes::replies_output_network, 0u, nodeset_names::agents, dense, nodeset_names::time, dense);

	Graph<unsigned int>* quotesOutputNetwork = construct.graph_manager.load_optional(attributes::quotes_output_network, 0u, nodeset_names::agents, dense, nodeset_names::time, dense);

	Graph<unsigned int>* likesOutputNetwork = construct.graph_manager.load_optional(attributes::likes_output_network, 0u, nodeset_names::agents, dense, nodeset_names::time, dense);
}

void createFollowerNet(Construct& construct, Nodeset* agentNodeset, dynet::ParameterMap generator_params)
{
	Graph<bool>* followerNetwork = construct.graph_manager.load_optional(graph_names::twit_follow, false, agentNodeset, dense, agentNodeset, sparse);
	generator_params.clear();
	generator_params["network name"] = attributes::network_all_communication;
	generator_params["file"] = ora_file_input;
	construct.graph_manager.generators.dynetml_generator(generator_params, followerNetwork);
}

void readAgentEngagementNetwork(Construct& construct, dynet::ParameterMap generator_params, Nodeset* agentNodeset, Nodeset* engagementNodeset)
{
	Graph<float>* engagementProbabilityNetwork = construct.graph_manager.load_optional(attributes::graph_engagement_probabilities, 0.0f, agentNodeset, dense, engagementNodeset, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_input_engagement;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, engagementProbabilityNetwork);
	}
	catch (std::exception e) {
		std::cout << e.what();
		exit(0);
	}
}

void readAgentAttributeNetworks(Construct& construct, dynet::ParameterMap generator_params, Nodeset* agentNodeset, Nodeset* attributesNodeset)
{
	Graph<float>* retweetProbabilityNetwork = construct.graph_manager.load_optional(attributes::graph_retweets, 0.0f, agentNodeset, dense, attributesNodeset, dense);
	generator_params.clear();
	generator_params["network name"] = attributes::network_input_retweets;
	generator_params["file"] = attributes::ora_file_input;
	try {
		construct.graph_manager.generators.dynetml_generator(generator_params, retweetProbabilityNetwork);
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
}