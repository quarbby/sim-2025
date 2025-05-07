#pragma once

namespace attributes
{
	// Input file
	const std::string ora_file_input = "twitter_truncated_for_construct_normalized_full.xml";
	const int timecount = 6;

	// Output Stuff - from Construct
	const std::string retweet_given_network = "retweet_given_network";
	const std::string replies_given_network = "replies_given_network";
	const std::string quotes_given_network = "quotes_given_network";
	const std::string likes_given_network = "likes_given_network";

	const std::string retweet_received_network = "retweet_received_network";
	const std::string replies_received_network = "replies_received_network";
	const std::string quotes_received_network = "quotes_received_network";

	const std::string output_dyxml = "output_events_network"; 
	const std::string output_posts_json = "output_posts";
	const std::string output_posts_frequency = "2";

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
}