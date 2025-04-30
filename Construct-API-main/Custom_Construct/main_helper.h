#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "namespace_attributes.h"

using namespace attributes;

Nodeset* initializeKnowledgeNodeset(Construct& construct);

Nodeset* initializeTimeNodeset(Construct& construct);

Nodeset* initializeAgentNodeset(Construct& construct);

Nodeset* initializeAttributesNodeset(Construct& construct);

void initializeOutputs(Construct& construct);

void addInteractionItemNames();