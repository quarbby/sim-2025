#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "namespace_attributes.h"

using namespace attributes;

const int KNOWLEDGE_COUNT = 5;

Nodeset* initializeKnowledgeNodeset(Construct& construct)
{
	Nodeset* knowledge_ns = construct.ns_manager.create_nodeset(nodeset_names::knowledge); 	//knowledge required regardless
	knowledge_ns->add_nodes(KNOWLEDGE_COUNT, dynet::ParameterMap());

	return knowledge_ns;

}

Nodeset* initializeTimeNodeset(Construct& construct)
{
	Nodeset* time_ns = construct.ns_manager.create_nodeset(nodeset_names::time);
	time_ns->add_nodes(construct.time_count, dynet::ParameterMap());

	return time_ns;

}

Nodeset* initializeAgentNodeset(Construct& construct)
{
	Nodeset* agent_ns = construct.ns_manager.create_nodeset(nodeset_names::agents);
	//agent_ns->import_dynetml(attributes::ora_file_input, attributes::nodeset_agent);
	agent_ns->import_dynetml(attributes::ora_file_input, attributes::nodeset_agent);

	return agent_ns;

}

Nodeset* initializeAttributesNodeset(Construct& construct)
{
	Nodeset* attributes_ns = construct.ns_manager.create_nodeset(attributes::nodeset_graph_attributes);
	attributes_ns->import_dynetml(attributes::ora_file_input, attributes::nodeset_graph_attributes);

	return attributes_ns;

}

void createKnowledgeNet(Construct& construct, Nodeset* agentNodeset, Nodeset* knowledgeNodeset)
{
	// default value of knowledge net = true means you know the particular piece of knowledge (or know about the hashtag)
	Graph<bool>* knowledge_net = construct.graph_manager.load_optional(graph_names::knowledge, true, agentNodeset, dense, knowledgeNodeset, sparse); 	
}

void initializeOutputs(Construct& construct)
{
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
}

void addInteractionItemNames() {
	InteractionItem::item_names[InteractionItem::item_keys::Lynnette_start] = attributes::itemkey_lynnette_start;
	InteractionItem::item_names[InteractionItem::item_keys::likes] = attributes::itemkey_likes;
	InteractionItem::item_names[InteractionItem::item_keys::retweets] = attributes::itemkey_retweets;
	InteractionItem::item_names[InteractionItem::item_keys::quotes] = attributes::itemkey_quotes;
	InteractionItem::item_names[InteractionItem::item_keys::reply] = attributes::itemkey_reply;
	InteractionItem::item_names[InteractionItem::item_keys::attributes] = attributes::itemkey_attributes;
	InteractionItem::item_names[InteractionItem::item_keys::Lynnette_end] = attributes::itemkey_lynnette_end;
}