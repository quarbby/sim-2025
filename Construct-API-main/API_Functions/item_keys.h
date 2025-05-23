#pragma once
#include "pch.h"

enum class InteractionItem::item_keys : char {
	knowledge,
	alter,
	belief,
	ktm,
	btm,
	ktrust,
	twitter_event,
	facebook_event,
	feed_position,
	emotion,
	banned,
	upvotes,
	downvotes,
	subreddit,
	prev_banned,
	reddit_event,
	//ordering of the above items shall not be modified
	//new items can be added after the above list
	//added items should be added to InteractionItem::item_names data structure

	Lynnette_start,
	likes,
	retweets,
	quotes,
	reply,
	bendE,
	Lynnette_end,

	BendE_start,
	back,
	build,
	bridge,
	boost,
	excite,
	engage,
	explain,
	enhance,
	neutralize,
	negate,
	narrow,
	neglect,
	dismay,
	dismiss,
	distort,
	distract,
	sad,
	fear,
	anger,
	happy,
	disgust,
	surprise,
	BendE_end


	//item_key list should not exceed 100 as this is reserved for the emotion nodeset
};
