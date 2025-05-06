#pragma once

namespace attributes
{
	// Input file
	const std::string ora_file_input = "twitter_truncated_for_construct_normalized_full.xml";

	// Output Stuff - from Construct
	const std::string retweet_output_network = "retweet_output_network"; 
	const std::string replies_output_network = "replies_output_network"; 
	const std::string quotes_output_network = "quotes_output_network"; 
	const std::string likes_output_network = "likes_output_network"; 

	const std::string output_dyxml = "output_events_network.xml"; // for ORA
	const std::string output_json = "output_events.json";
	

 	// Input Networks - from ORA
	const std::string network_all_communication = "Agent_Agent_All_Communication";

	const std::string network_input_retweets = "Agent_Attributes_Retweet";
	const std::string network_input_replies = "Agent_Attributes_Reply";
	const std::string network_input_quotes = "Agent_Attributes_Quote";
	const std::string network_input_likes = "Agent_Attributes_Likes";
	const std::string network_input_probabilities = "Agent_Attributes_Probability";
	const std::string network_input_engagement = "Agent_Engagement_Probability";

	// Input Networks Names - for Construct
	const std::string graph_agent_attributes = "graph_agent_attributes_network";
	const std::string graph_retweets = "graph_agent_attributes_retweet";
	const std::string graph_replies = "graph_agent_attributes_replies";
	const std::string graph_quotes = "graph_agent_attributes_quotes";
	const std::string graph_likes = "graph_agent_attributes_likes";
	const std::string graph_bende_probabilities = "graph_bende_probabilities";
	const std::string graph_engagement_probabilities = "graph_engagement_probabilities";

	// Nodeset
	const std::string nodeset_agent = "Agent";
	const std::string nodeset_engagement = "Engagement";

	const std::string nodeset_graph_agent = "agent";
	const std::string nodeset_graph_attributes = "Attributes";
	const std::string nodeset_graph_engagement = "Engagement";

	// Item key names 
	const std::string itemkey_lynnette_start = "Lynnette_start";
	const std::string itemkey_likes = "likes";
	const std::string itemkey_retweets = "retweets";
	const std::string itemkey_quotes = "quotes";
	const std::string itemkey_reply = "reply";
	const std::string itemkey_bendE = "bendE";
	const std::string itemkey_lynnette_end = "Lynnette_end";
	
	const std::string itemkey_bende_start = "BendE_start";
	const std::string itemkey_willRetweet = "will_retweet";
	const std::string itemkey_willReply = "will_reply";
	const std::string itemkey_willQuote = "will_quote";
	const std::string itemkey_bende_end = "BendE_end";
}