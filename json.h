
#pragma once

#include "picojson.h"
#include <string>
#include <sstream>

using namespace std;
using namespace picojson;

class Json {
	private:
		value v;
	public:
		Json(value json) {
			v = json;
		}
		Json(string json) {
			parse(v, json);
		}
		void reparse(string json) {
			parse(v, json);
		}
		operator value &() {
			return v;
		}
		value value() {
			return v;
		}
		Json operator[](string key) {
			return Json(v.get<object>()[key]);
		}
		Json operator[](size_t key) {
			return Json(v.get<picojson::array>()[key]);
		}
		size_t size() {
			return v.get<picojson::array>().size();
		}
		double as_double() {
			return v.get<double>();
		}
		int as_int() {
			return ceil(v.get<double>());
		}
		bool as_bool() {
			return v.get<bool>();
		}
		string as_string() {
			return v.get<string>();
		}
};
