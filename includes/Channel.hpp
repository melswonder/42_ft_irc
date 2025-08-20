#pragma once

#include "IRC.hpp"

class Channel {
	private:
		std::string _name;
		std::string _topic;
		std::string _key;
		std::set<Client*> _members;
		std::set<Client*> _operators;
		std::set<Client*> _inviteList;
		bool _inviteOnly;
		bool _topicRestricted;
		int _userLimit;
		
	public:
		Channel(const std::string& name);
		~Channel();
		
		// Getters
		const std::string& getName() const;
		const std::string& getTopic() const;
		const std::string& getKey() const;
		const std::set<Client*>& getMembers() const;
		const std::set<Client*>& getOperators() const;
		bool isInviteOnly() const;
		bool isTopicRestricted() const;
		int getUserLimit() const;
		
		// Setters
		void setTopic(const std::string& topic);
		void setKey(const std::string& key);
		void setInviteOnly(bool inviteOnly);
		void setTopicRestricted(bool topicRestricted);
		void setUserLimit(int limit);
		
		// Member management
		void addMember(Client* client);
		void removeMember(Client* client);
		bool isMember(Client* client) const;
		
		// Operator management
		void addOperator(Client* client);
		void removeOperator(Client* client);
		bool isOperator(Client* client) const;
		
		// Invite management
		void addToInviteList(Client* client);
		void removeFromInviteList(Client* client);
		bool isInvited(Client* client) const;
		
		// Utility
		bool isEmpty() const;
		bool canJoin(Client* client, const std::string& key = "") const;
		std::string getMembersList() const;
	};
