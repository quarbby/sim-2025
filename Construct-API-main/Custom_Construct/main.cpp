# pragma once

#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>

#include "attributes.h"
#include "bendEmotions.h"

#include "MainHelper.h"

#include "TwitterFormulation1.h"
#include "TwitterFormulation2.h"
#include "TwitterFormulation3.h"

using namespace attributes;
using namespace bendEmotions;

#define TWITTERFORMULATION 3
#define TIME_COUNT 2

#define DELTA_T 1.0	//in hours

int main() 
{
	try 
	{
		// Construct initialization
		Construct construct;
		construct.verbose_runtime = true;
		construct.random.set_seed(time(nullptr));
		construct.time_count = TIME_COUNT;

		dynet::ParameterMap generatorParams;

		// Initialize nodesets
		// Project specific
		Nodeset* agentNodeset = initializeAgentNodeset(construct);
		Nodeset* attributesNodeset = initializeAttributesNodeset(construct);
		Nodeset* engagementNodeset = initializeEngagementNodeset(construct);

		// Construct specific
		Nodeset* knowledgeNodeset = initializeKnowledgeNodeset(construct);
		Nodeset* timeNodeset = initializeTimeNodeset(construct);

		// Initialize graphs
		createKnowledgeNet(construct, agentNodeset, knowledgeNodeset);
		createFollowerNet(construct, agentNodeset, generatorParams);

		readAgentAttributeNetworks(construct, generatorParams, agentNodeset, attributesNodeset);
		readAgentEngagementNetwork(construct, generatorParams, agentNodeset, engagementNodeset);

		createOutputNetworks(construct);

		//Add item names 
		addInteractionItemNames();

		// Adding the twitter model, additional models can be added here
		// Add models after loading all the nodesets and networks
		dynet::ParameterMap modelParams;
		modelParams["interval time duration"] = std::to_string(DELTA_T);
		modelParams["maximum post inactivity"] = "24";

		Model* model;

		try 
		{
			if (TWITTERFORMULATION == 1)
			{
				Lynnette_Twitter* lynnetteTwitter = new Lynnette_Twitter(modelParams, construct);
				lynnetteTwitter->setupNetwork();
				model = (Model*)lynnetteTwitter;
			}
			else if (TWITTERFORMULATION == 2)
			{
				TwitterFormulation2* lynnetteTwitter = new TwitterFormulation2(modelParams, construct);
				lynnetteTwitter->setupNetwork();
				model = (Model*)lynnetteTwitter;
			}
			else if (TWITTERFORMULATION == 3)
			{
				TwitterFormulation3* lynnetteTwitter = new TwitterFormulation3(modelParams, construct);
				lynnetteTwitter->setupNetwork();
				model = (Model*)lynnetteTwitter;
			}
		}
		catch (std::exception e) 
		{
			std::cout << e.what();
			exit(0);
		}

		construct.model_manager.add_model(model_names::TWIT_wf, model);

		initializeOutputs(construct);

		construct.run();
	}
	catch (dynet::construct_exception e) 
	{
		std::cout << e.what();
	}

}