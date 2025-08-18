#include "IRC.hpp"

Channel::Channel(const std::string& name) 
    : _name(name), _inviteOnly(false), _topicRestricted(true), _userLimit(0) {
}

Channel::~Channel() {
}

const std::string& Channel::getName() const {
	return _name;
}

const std::string& Channel::getTopic() const {
	return _topic;
}

const std::string& Channel::getKey() const {
	return _key;
}

const std::set<Client*>& Channel::getMembers() const {
	return _members;
}

const std::set<Client*>& Channel::getOperators() const {
	return _operators;
}

bool Channel::isInviteOnly() const {
	return _inviteOnly;
}

bool Channel::isTopicRestricted() const {
	return _topicRestricted;
}

int Channel::getUserLimit() const {
	return _userLimit;
}

void Channel::setTopic(const std::string& topic) {
	_topic = topic;
}

void Channel::setKey(const std::string& key) {
	_key = key;
}

void Channel::setInviteOnly(bool inviteOnly) {
	_inviteOnly = inviteOnly;
}

void Channel::setTopicRestricted(bool topicRestricted) {
	_topicRestricted = topicRestricted;
}

void Channel::setUserLimit(int limit) {
	_userLimit = limit;
}

void Channel::addMember(Client* client) {
	_members.insert(client);
	client->joinChannel(_name);

	// First member becomes operator
	if (_members.size() == 1) {
		addOperator(client);
	}
}

void Channel::removeMember(Client* client) {
	_members.erase(client);
	_operators.erase(client);
	_inviteList.erase(client);
	client->leaveChannel(_name);
}

bool Channel::isMember(Client* client) const {
	return _members.find(client) != _members.end();
}

void Channel::addOperator(Client* client) {
	if (isMember(client)) {
		_operators.insert(client);
	}
}

void Channel::removeOperator(Client* client) {
	_operators.erase(client);
}

bool Channel::isOperator(Client* client) const {
	return _operators.find(client) != _operators.end();
}

void Channel::addToInviteList(Client* client) {
	_inviteList.insert(client);
}

void Channel::removeFromInviteList(Client* client) {
	_inviteList.erase(client);
}

bool Channel::isInvited(Client* client) const {
	return _inviteList.find(client) != _inviteList.end();
}

bool Channel::isEmpty() const {
	return _members.empty();
}

bool Channel::canJoin(Client* client, const std::string& key) const {
	// Check user limit
	if (_userLimit > 0 && static_cast<int>(_members.size()) >= _userLimit) {
		return false;
	}

	// Check key
	if (!_key.empty() && _key != key) {
		return false;
	}

	// Check invite-only
	if (_inviteOnly && !isInvited(client)) {
		return false;
	}

	return true;
}

std::string Channel::getMembersList() const {
	std::stringstream ss;
	for (std::set<Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
		if (it != _members.begin()) {
			ss << " ";
		}
		if (isOperator(*it)) {
			ss << "@";
		}
		ss << (*it)->getNickname();
	}
	return ss.str();
}
