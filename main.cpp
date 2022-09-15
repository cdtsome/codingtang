
#include <bits/stdc++.h>
#include <conio.h>
#include "http.h"
#include "json.h"
#include "ocr.h"

using namespace std;

void cls() {
	system("cls");
}
void title(string tit) {
	SetConsoleTitle(tit.c_str());
}
void gotoxy(short x, short y) {
	COORD coord = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
string RandString(size_t size) {
	string ret;
	for (size_t i = 0; i < size; i++) {
		ret += rand() % 26 + 'a';
	}
	return ret;
}
string ReadALL(string fname) {
	ifstream st(fname.c_str());
	string stin((istreambuf_iterator<char>(st)), (istreambuf_iterator<char>()));
	return stin;
}

Http http;

void init() {
	title("�������ڳ�ʼ����...");
	gotoxy(0, 0);
	cout << "�������ڳ�ʼ�������Ժ�(1/2)...";
	thread descrypt([]() {
		system("descrypt.exe");
	});
	descrypt.detach();
	gotoxy(0, 0);
	cout << "�������ڳ�ʼ�������Ժ�(2/2)...";
	freopen("stderr.log", "w", stderr);
	initOCR();
	cls();
}
string descrypt(const string &str) {
	return http.Post("http://127.0.0.1:19198", str);
}
void Utf8ToAnsi(Json &json) {
	json = Json(http.ToAnsi(json.value().serialize()));
}
bool havefile(string filename) {
	ifstream ff(filename);
	return ff.good();
}
string get(string url, string token) {
	return http.Get(url, "authorization: Token " + token);
}
string post(string url, const string &txt, string token) {
	return http.Post(url, txt, "authorization: Token " + token);
}
void getdata() {
	cls();
	cout << "��ȡ������...";
}
string table(string a, int size) {
	char ret[size + 1] = {};
	for (int i = 0; i < size; i++) {
		ret[i] = ' ';
	}
	memcpy(ret, a.c_str(), min(size, int(a.size())));
	return ret;
}

void devTools(string token) {
	cls();
	title("�����߹���");
	while (1) {
		int ipt = getch();
		if (ipt == 'Q' || ipt == 'q') {
			break;
		} else if (ipt == 'G' || ipt == 'g') {
			string url;
			getline(cin, url);
			cout << get(url, token);
		} else if (ipt == 'P' || ipt == 'p') {
			string url, content;
			getline(cin, url);
			getline(cin, content);
			cout << post(url, content, token);
		}
	}
	cls();
}

void problemList(string token) {
	title("��� - �����");
	int page = 1;
	while (1) {
		getdata();
		Json json(get("https://courseadmin.codingtang.com/problem/list?judge_type=&key=&tag=&type=&page=" + to_string(page) +
		              "&size=20&field=&rise=&experience=&complete=&is_lesson_content_problem=", token));
		json = Json(descrypt(json["data"].as_string()));
		Utf8ToAnsi(json);
		json = json["problems"]["data"];
		cls();
		cout << " ��� | ����                 | �Ѷ� | ����ȼ� | �ҵĵ÷�\n";
		cout << "����������������������������������������������������������\n";
		for (size_t ii = 0; ii < json.size(); ii++) {
			Json i = json[ii];
			printf("%5d | %s | %s | %s | %s\n",
			       i["id"].as_int(),
			       table(i["title"].as_string(), 20).c_str(),
			       table(to_string(i["rank"].as_int()), 4).c_str(),
			       table(i["type_str"].as_string(), 8).c_str(),
			([&i]()->string {
				int sc = i["score"].as_int();
				if (sc == -1) {
					return "��";
				} else if (sc == 100) {
					return "AC";
				} else {
					return to_string(sc);
				}
			})().c_str()
			      );
		}
		printf("��������������������������������������������������%04dҳ��\n", page);
		cout << " ��'O'����Ŀ��'N'��һҳ��'P'��һҳ��'S'������'Q'�˳�\n";
		cout << ">>";
		int ipt = getch();
		if (ipt == 'N' || ipt == 'n') {
			page++;
		} else if (ipt == 'P' || ipt == 'p') {
			page--;
		} else if (ipt == 'Q' || ipt == 'q') {
			break;
		}
	}
	cls();
}

void home(string token) {
	while (1) {
		title("��ҳ - �����");
		getdata();
		Json json(get("https://courseadmin.codingtang.com/course/home", token));
		json = json["data"];
		cls();
		cout << "�û�����" << json["user_data"]["user_name"].as_string() << "��" << json["user_data"]["user_id"] << "��\n";
		cout << "�ȼ���" << json["level_data"]["level_name"].as_string() << "��Lv" << json["level_data"]["my_level"] << "��\n";
		double bfb = json["level_data"]["my_level_experience"].as_double() / json["level_data"]["level_experience"].as_double();
		cout << "      ";
		for (int i = 1; i <= 10; i++) {
			if (i <= ceil(bfb * 10)) {
				cout << "��";
			} else {
				cout << "��";
			}
		}
		printf("%.2f%\n", bfb * 100);
		printf("��������%d\t | ��������%d\n", json["level_data"]["my_rank"].as_int(), json["level_data"]["month_rank"].as_int());
		printf("ս��ֵ��%d\t | ����ѧУս����%d\n", json["user_data"]["power"].as_int(), json["user_data"]["school_power"].as_int());
		printf("���Եȼ���%d\t | ���Ծ��飺%d\n", json["debug"]["debug_level"].as_int(), json["debug"]["debug_experience"].as_int());
		printf("\nѡ��\n");
		printf("1. �༶\n");
		printf("2. ���\n");
		printf(">>");
		int ipt = getch();
		if (ipt == '1') {

		} else if (ipt == '2') {
			problemList(token);
		} else if (ipt == 'D' || ipt == 'd') {
			devTools(token);
		}
	}
}

void login() {
	cls();
	title("��¼ - �����");
	auto checkToken = [](const string &tok) {
		Json json(get("https://courseadmin.codingtang.com/course/auth/userinfo", tok));
		if (json.value().get<object>()["detail"].is<string>()) {
			return false;
		} else {
			return true;
		}
	};
	if (havefile("lastToken")) {
		cls();
		cout << "����ʹ���¼...";
		string tok = ReadALL("lastToken");
		if (checkToken(tok)) {
			home(tok);
		}
	}
	auto login1 = []() {
		cls();
		cout << "�˺ţ�\n���룺\n";
		gotoxy(6, 0);
		string username, password;
		getline(cin, username);
		gotoxy(6, 1);
		getline(cin, password);
		cls();
		cout << "����ʹ���¼...";
		string cid = RandString(16), fn = RandString(12);
		http.Download("https://courseadmin.codingtang.com/course/auth/captcha?v=0&cid=" + cid, "temp/" + fn);
		string imgcode = doOCR("temp/" + fn);
		remove(string("temp/" + fn).c_str());
		string cont = "cid=" + cid + "&imageCode=" + imgcode + "&password=" + password + "&username=" + username;
		Json json(http.Post("https://courseadmin.codingtang.com/course/auth/login_", cont));
		if (json["code"].as_double() == 0) {
			ofstream tt("lastToken");
			tt << json["data"]["access_token"].as_string();
			tt.close();
			home(json["data"]["access_token"].as_string());
		} else {
			cls();
			cout << "��¼ʧ�ܣ�" << json["msg"].as_string() << endl;
			Sleep(1000);
		}
	};
	auto login2 = [&checkToken]() {
		cls();
		cout << "���ƣ�";
		string tok;
		cin >> tok;
		if (checkToken(tok)) {
			ofstream tt("lastToken");
			tt << tok;
			tt.close();
			home(tok);
		} else {
			cls();
			cout << "��¼ʧ�ܣ�������Ч" << endl;
			Sleep(1000);
		}
	};
	while (1) {
		cls();
		cout << "ѡ���¼��ʽ��\n";
		cout << "1. �˺������¼\n";
		cout << "2. ���Ƶ�¼\n";
		cout << ">>";
		int ipt = getch();
		if (ipt == '1') {
			login1();
		} else if (ipt == '2') {
			login2();
		}
	}
}

void start() {
	cls();
	login();
}

int main() {
	try {
		init();
		start();
	} catch (exception &e) {
		string warn = "�ǳ���Ǹ���˳����׳���һ���쳣���Ƿ���Ը��쳣������ִ��";
		warn += "��ע�⣺�����쳣������ִ�п��ܻᵼ�²���Ԥ�ϵĴ��󣩣�\n�쳣���ͣ�";
		warn += e.what(), warn += "\n�������򿪷��߱����������";
		if (MessageBox(NULL, warn.c_str(), "�쳣", MB_ICONERROR | MB_YESNO) == IDNO) {
			exit(-1);
		}
	} catch (...) {
		string warn = "�ǳ���Ǹ���˳����׳���һ��δ֪���쳣���Ƿ���Ը��쳣������ִ��";
		warn += "��ע�⣺�����쳣������ִ�п��ܻᵼ�²���Ԥ�ϵĴ��󣩣�\n";
		warn += "\n�������򿪷��߱����������";
		if (MessageBox(NULL, warn.c_str(), "�쳣", MB_ICONERROR | MB_YESNO) == IDNO) {
			exit(-1);
		}
	}
	return 0;
}
