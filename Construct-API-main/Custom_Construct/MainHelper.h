#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "attributes.h"

using namespace attributes;

Nodeset* initializeKnowledgeNodeset(Construct& construct);

Nodeset* initializeTimeNodeset(Construct& construct);

Nodeset* initializeAgentNodeset(Construct& construct);

Nodeset* initializeAttributesNodeset(Construct& construct);

void initializeOutputs(Construct& construct);

void addInteractionItemNames();

void createKnowledgeNet(Construct& construct, Nodeset* agentNodeset, Nodeset* knowledgeNodeset);

void createOutputNetworks(Construct& construct);

void createFollowerNet(Construct& construct, Nodeset* agentNodeset, dynet::ParameterMap generator_params);

void readAgentAttributeNetworks(Construct& construct, dynet::ParameterMap generator_params, Nodeset* agentNodeset, Nodeset* attribtuesNodeset);