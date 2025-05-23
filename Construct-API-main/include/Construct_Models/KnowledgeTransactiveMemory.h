#pragma once
#include "pch.h"

//This model operates as a modification to the Standard Interaction Model and thus requires the header
#include "StandardInteraction.h"

class KnowledgeTransactiveMemory : public StandardInteraction
{
	const std::string send_ktm = "can send knowledgeTM";
	const std::string recv_ktm = "can receive knowledgeTM";
public:
	//graph name - "transactive knowledge message complexity network"
	//agent x timeperiod
	const Graph<unsigned int>& tmk_message_complexity = graph_manager.load_optional(graph_names::ktm_msg_complex, 1u, agents, sparse, time, sparse);
	//graph name - "agent group membership network"
	//agent x agentgroup
	const Graph<bool>* group_membership;
	//graph name "agent group knowledge network"
	//agentgroup x knowledge
	Graph<float>* group_knowledge;
	//graph name "knowledge transactive memory network"
	//agent x agent x knowledge
	Graph<std::vector<bool> >& tmk = graph_manager.load_optional(graph_names::ktm, std::vector<bool>(agents.size(), false), agents, dense, knowledge, sparse);
	unsigned int group_count;

	std::vector<unsigned int> group_size;

	std::vector<float> generalized_other;
	
	KnowledgeTransactiveMemory(const dynet::ParameterMap& parameters, Construct& construct);
	
	void initialize(void) override;

	void communicate(const InteractionMessage& msg) override;

	void cleanup() override;

	std::vector<InteractionItem> get_interaction_items(unsigned int sndr, unsigned int recv, const CommunicationMedium* comm) override;

	float get_k_sim(unsigned int agent_i, unsigned int agent_j) override;

	float get_k_exp(unsigned int agent_i, unsigned int agent_j) override;

	virtual void update_group_knowledge(void);
};
