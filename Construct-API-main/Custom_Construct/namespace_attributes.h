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
	const std::string network_all_communication = "Agent x Agent - All Communication";

	const std::string network_agent_attributes_retweets = "Agent_Attributes_Retweet";
	const std::string network_agent_attributes_replies = "Agent x Attributes - Reply";
	const std::string network_agent_attributes_quotes = "Agent x Attributes - Quote";
	const std::string network_agent_attributes_likes = "Agent x Attributes - Likes";

	const std::string network_agent_attributes_probabilities = "Agent x Attributes - Probability";

	// Input Networks - for Construct
	const std::string graph_agent_attributes = "agent_attributes_network";

	// Nodeset

	const std::string nodeset_agent = "Agent";

	const std::string nodeset_attributes = "Attributes";

	const std::string nodeset_graph_agent = "agent";

	const std::string nodeset_graph_attributes = "Attributes";

	// Item key names 
	const std::string itemkey_lynnette_start = "Lynnette_start";
	const std::string itemkey_likes = "likes";
	const std::string itemkey_retweets = "retweets";
	const std::string itemkey_quotes = "quotes";
	const std::string itemkey_reply = "reply";
	const std::string itemkey_attributes = "attributes";
	const std::string itemkey_lynnette_end = "Lynnette_end";
	
}