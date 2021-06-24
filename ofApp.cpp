
#include "ofApp.h"	

//--------------------------------------------------------------
Actor::Actor(vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	this->select_index = ofRandom(location_list.size());
	while (true) {

		auto itr = find(destination_list.begin(), destination_list.end(), this->select_index);
		if (itr == destination_list.end()) {

			destination_list.push_back(this->select_index);
			break;
		}

		this->select_index = (this->select_index + 1) % location_list.size();
	}

	this->next_index = this->select_index;
}

//--------------------------------------------------------------
void Actor::update(const int& frame_span, vector<glm::vec3>& location_list, vector<vector<int>>& next_index_list, vector<int>& destination_list) {

	if (ofGetFrameNum() % frame_span == 0) {

		auto tmp_index = this->select_index;
		this->select_index = this->next_index;
		int retry = next_index_list[this->select_index].size() + 1;
		this->next_index = next_index_list[this->select_index][(int)ofRandom(next_index_list[this->select_index].size())];
		while (--retry > 0) {

			auto destination_itr = find(destination_list.begin(), destination_list.end(), this->next_index);
			if (destination_itr == destination_list.end()) {

				if (tmp_index != this->next_index) {

					destination_list.push_back(this->next_index);
					break;
				}
			}

			this->next_index = next_index_list[this->select_index][(this->next_index + 1) % next_index_list[this->select_index].size()];
		}
		if (retry <= 0) {

			destination_list.push_back(this->select_index);
			this->next_index = this->select_index;
		}
	}

	auto param = ofGetFrameNum() % frame_span;
	auto distance = location_list[this->next_index] - location_list[this->select_index];
	if (this->next_index != this->select_index) {

		this->prev_location = location;
	}
	this->location = location_list[this->select_index] + distance / frame_span * param;
}

//--------------------------------------------------------------
glm::vec3 Actor::get_location() {

	return this->location;
}

//--------------------------------------------------------------
glm::vec3 Actor::get_prev_location() {

	return this->prev_location;
}

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetColor(39);
	ofSetLineWidth(2);
	ofNoFill();

	this->span = 50;
	for (int x = -200; x <= 200; x += span) {

		for (int y = -200; y <= 200; y += span) {

			this->location_list.push_back(glm::vec3(x, y, 0));
		}
	}

	for (auto& location : this->location_list) {

		vector<int> next_index = vector<int>();
		int index = -1;
		for (auto& other : this->location_list) {

			index++;
			if (location == other) { continue; }

			float distance = glm::distance(location, other);
			if (distance <= span + 1) {

				next_index.push_back(index);
			}
		}

		this->next_index_list.push_back(next_index);
	}

	for (int i = 0; i < 60; i++) {

		this->actor_list.push_back(make_unique<Actor>(this->location_list, this->next_index_list, this->destination_list));
	}

	ofTrueTypeFontSettings font_settings("fonts/msgothic.ttc", 25);
	font_settings.antialiased = true;
	font_settings.addRanges(ofAlphabet::Japanese);
	this->font.load(font_settings);

	this->word_list = {

		u8"—ë", u8"ˆë", u8"“ó", u8"ŽQ", u8"Žl",
		u8"ŒÞ", u8"˜Z", u8"Žµ", u8"”ª", u8"‹ã",
	};
}

//--------------------------------------------------------------
void ofApp::update() {

	int frame_span = 15;
	int prev_index_size = 0;

	if (ofGetFrameNum() % frame_span == 0) {

		prev_index_size = this->destination_list.size();
	}

	for (auto& actor : this->actor_list) {

		actor->update(frame_span, this->location_list, this->next_index_list, this->destination_list);
	}

	if (prev_index_size != 0) {

		this->destination_list.erase(this->destination_list.begin(), this->destination_list.begin() + prev_index_size);
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	ofTranslate(ofGetWindowSize() * 0.5);

	vector<string> word_list;

	for (auto& actor : this->actor_list) {

		int count = 0;
		for (auto& other : this->actor_list) {

			if (actor == other) { continue; }

			if (glm::distance(actor->get_location(), other->get_location()) <= this->span * sqrt(2)) {

				count++;
			}
		}

		this->font.drawString(this->word_list[count], actor->get_location().x - this->font.stringWidth(this->word_list[count]) * 0.5, actor->get_location().y + this->font.stringHeight(this->word_list[count]) * 0.5);
	}

	for (int x = -200; x <= 200; x += this->span) {

		for (int y = -200; y <= 200; y += this->span) {

			ofDrawRectangle(glm::vec3(x - this->span * 0.5, y - this->span * 0.5, 0), this->span, this->span);
		}
	}	
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}