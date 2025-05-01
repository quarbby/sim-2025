#include "pch.h"
#include "Output.h"
#include "SocialMedia.h"
#include "item_keys.h"
#include <cmath>
#include "attributes.h"

using namespace attributes;

const double PI = 3.14159265358979323846;

struct TwitterFormulation2 : public Twitter_wf {
	struct LynnetteUser : public Social_Media_with_followers::default_media_user
	{
		LynnetteUser(Social_Media_with_followers* media, const Node& node);

		TwitterFormulation2& media();

		float getUserAttributes(Graph<float>* agentAttributeNetwork);

		// If you want to edit repost probabilities, edit this function; then remember to edit output info
		void repost(media_event* _event) override;

		void quote(media_event* _event) override;

		void reply(media_event* _event) override;

		float sigmoidActivation(float x);

		float atanClamp(float x);

		// increase likes by the probability of the attributes additions
		int likesIncrease(Social_Media_no_followers::media_event* me);

		void parse(Social_Media_no_followers::media_event* me) override;

		void enrich_event(Social_Media_no_followers::media_event* me) override;
	};


	TwitterFormulation2(const dynet::ParameterMap& parameters, Construct& construct);

	Social_Media_no_followers::media_event* create_post(unsigned int knowledge, unsigned int id) override;

	Graph<float>* retweetAttributeNetwork = nullptr;
	Graph<float>* replyAttributeNetwork = nullptr;
	Graph<float>* quotesAttributeNetwork = nullptr;
	Graph<float>* likesAttributeNetwork = nullptr;
	Graph<unsigned int>* retweetOutputNetwork = nullptr;
	Graph<unsigned int>* repliesOutputNetwork = nullptr;
	Graph<unsigned int>* quotesOutputNetwork = nullptr;
	Graph<unsigned int>* likesOutputNetwork = nullptr;

	void setupNetwork();

	

};