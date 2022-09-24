/* require modules */
const fs = require('fs');
const os = require('os');
const cp = require('child_process');
const path = require('path');
const request = require('request');
const cdt = require('./crypt/crypt.js');
const ct = require('@xdooi/consoletable');
const marked = require('marked');

/* pre consts */
const getdata = '获取数据中...';

/* pre functions */
function print(args) {
	console.log(args);
}
function input() {
	return new Promise(function(resolve, reject) {
		const readline = require('readline').createInterface({
			input: process.stdin,
			output: process.stdout
		});
		readline.question('', function(res) {
			readline.close();
			resolve(res);
		});
	});
}
function cls() {
	console.clear();
}
function download(url, filename) {
	return new Promise(function(resolve, reject) {
		var stream = fs.createWriteStream(filename);
		request(url).pipe(stream).on('close', function() {
			resolve();
		});
	});
}
function runFile(filename) {
	if (os.platform() == 'win32') {
		cp.exec(filename);
	} else {
		cp.exec('xdg-open ' + filename);
	}
}
function get(url, body, token = '') {
	return new Promise(function(resolve, reject) {
		request.get(
			{
				url: url,
				qs: body,
				headers: {
		    		'authorization': 'Token ' + token
				}
			}, 
			function(error, response, body) {
				if (!error) {
					resolve(body);
				}
			}
		); 
	});
}
function post(url, body, token = '') {
	return new Promise(function(resolve, reject) {
		request.post(
			{
				url: url,
				form: body,
				headers: {
		    		'authorization': 'Token ' + token
				}
			}, 
			function(error, response, body) {
				if (!error) {
					resolve(body);
				}
			}
		); 
	});
}

/* program functions */
async function init() {
	await fs.mkdir(path.join(process.cwd(), './temp'), function(err) {});
}
function check(token) {
	return new Promise(function(resolve, reject) {
		get(
			'https://courseadmin.codingtang.com/course/auth/userinfo', null, token
		).then(function(body) {
			var json = JSON.parse(body);
			resolve(json.code == 0);
		});
	});
}
async function login() {
	await fs.access(path.join(process.cwd(), './lastToken'), fs.constants.R_OK, function(err) {
		function slogin() {
			cls();
			print('登录方式：');
			print('1. 密码登录');
			print('2. 令牌登录');
			print('>>>');
			input().then(function(res) {
				async function login1() {
					cls();
					print('输入账号：');
					var username, password, imgcode;
					await input().then(function(res) {
						username = res;
					});
					print('输入密码：');
					await input().then(function(res) {
						password = res;
					});
					cls();
					print(getdata);
					var cid = Math.random().toString(36).slice(-8);
					var imgurl = 'https://courseadmin.codingtang.com/course/auth/captcha?v=0&cid=' + cid;
					await download(
						imgurl, path.join(process.cwd(), './temp/' + cid + '.png')
					).then(function() {
						runFile(path.join(process.cwd(), './temp/' + cid + '.png'));
					});
					cls();
					print('输入验证码：');
					await input().then(function(res) {
						imgcode = res;
						fs.unlink(
							path.join(process.cwd(), './temp/' + cid + '.png'), 
							function(err) {}
						);
					});
					cls();
					print(getdata);
					await post(
						'https://courseadmin.codingtang.com/course/auth/login_',
						{
							'cid': cid,
							'imageCode': imgcode,
							'password': password,
							'username': username
						}
					).then(function(body) {
						var json = JSON.parse(body);
						cls();
						if (json.code != 0) {
							print('登录失败，原因：' + json.msg);
							setTimeout(login, 1000);
						} else {
							home(json.data.access_token);
						}
					});
				}
				async function login2() {
					cls();
					print('输入令牌：');
					var token;
					await input().then(function(res) {
						token = res;
					});
					cls();
					print(getdata);
					await check(token).then(function(ok) {
						if (ok) {
							home(token);
						} else {
							print('登录失败，原因：令牌错误');
							setTimeout(login, 1000);
						}
					});
				}
				if (res == '1') {
					login1();
				} else if (res == '2') {
					login2();
				} else {
					login();
				}
			});
		};
  		if (!err) {
			cls();
			print('尝试使你登录...');
			var token = fs.readFileSync(path.join(process.cwd(), './lastToken'), 'utf8');
			(async function() {
				await check(token).then(function(ok) {
					if (ok) {
						home(token);
					} else {
						fs.unlink(path.join(process.cwd(), './lastToken'), function(err) {});
						slogin();
					}
				});
			})();
  		} else {
  			slogin();
  		}
	});
}
function home(token) {
	cls();
	fs.writeFileSync(path.join(process.cwd(), './lastToken'), token);
	print(getdata);
	get(
		'https://courseadmin.codingtang.com/course/home', null, token
	).then(function(body) {
		var json = JSON.parse(body);
		json = json.data;
		cls();
		print(`用户名：${json.user_data.user_name}（${json.user_data.user_id}）`);
		print(`等级：Lv.${json.level_data.my_level} ${json.level_data.level_name}`);
		var bfb = json.level_data.my_level_experience / json.level_data.level_experience;
		var blocksize = Math.floor(bfb * 20);
		var blocks = '';
		for (var i = 1; i <= blocksize; i++) {
			blocks += '@';
		}
		for (var i = blocksize + 1; i < 20; i++) {
			blocks += '=';
		}
		print(`      [${blocks}] ${(bfb * 100).toFixed(2)}%`);
		print(`总排名：${json.level_data.my_rank}\t | 月排名：${json.level_data.month_rank}`);
		print(`酷町币：${json.user_data.coin}\t | 战力值：${json.user_data.power}`);
		print(`调试等级：${json.debug.debug_level}\t | 调试经验：${json.debug.debug_experience}`);
		print('');
		print('选择：');
		print('1. 班级');
		print('2. 题库');
		print('>>>');
		input().then(function(res) {
			if (res == '1') {
			
			} else if (res == '2') {
				problemList(token, 1, '');
			} else {
				home(token);
			}
		});
	});
}
function problemList(token, page, key) {
	cls();
	print(getdata);
	get(
		'https://courseadmin.codingtang.com/problem/list',
		{
			'judge_type': null,
			'key': key,
			'tag': null,
			'type': null,
			'page': page,
			'size': 20,
			'field': null,
			'rise': null,
			'experience': null,
			'complete': null,
			'is_lesson_content_problem': null
		},
		token
	).then(function(body) {
		var json = JSON.parse(body);
		json = JSON.parse(cdt.descrypt(json.data));
		cls();
		var table = [{
			'id': '题号',
			'title': '标题',
			'rank': '难度',
			'type': '怪物等级',
			'score': '我的得分'
		}];
		var list = json.problems.data;
		for (var i in list) {
			table.push({
				'id': list[i].id,
				'title': list[i].title,
				'rank': list[i].rank,
				'type': list[i].type_str,
				'score': list[i].score == -1 ? '未做题' : list[i].score
			});
		}
		ct.drawTable([{
			'page': `第 ${page} 页`,
			'next': '下一页(N)',
			'prev': '上一页(P)',
			'open': '打开(O)',
			'back': '返回(B)',
			'search': `搜索(S)：[${key}]`
		}], {head: false});
		ct.drawTable(table, {head: 'def'});
		print('>>>');
		input().then(function(res) {
			if (res == 'N' || res == 'n') {
				problemList(token, page + 1, key);
			} else if (res == 'P' || res == 'p') {
				problemList(token, page - 1, key);
			} else if (res == 'O' || res == 'o') {
				cls();
				print('题号：');
				input().then(function(res) {
					problemSub(token, res);
				});
			} else if (res == 'B' || res == 'b') {
				home(token);
			} else if (res == 'S' || res == 's') {
				cls();
				print('搜索：');
				input().then(function(res) {
					problemList(token, 1, res);
				});
			} else {
				problemList(token, page, key);
			}
		});
	});
}
function problemSub(token, id) {
	cls();
	print(getdata);
	get(
		'https://courseadmin.codingtang.com/problem/detail',
		{'id': id}, token
	).then(function(body) {
		var json = JSON.parse(body);
		json = JSON.parse(cdt.descrypt(json.data));
		json = JSON.parse(cdt.descrypt(json.problem));
		cls();
		ct.drawTable([
			{
				'type': `${json.id}`,
				'content': `${json.title}     ` +
						   `经验值：${json.experience}  时间限制：${json.time_limit}  内存限制：${json.memory_limit}`
			},
			{
				'type': '题目描述',
				'content': marked.parse(json.description).replace(/<[^>]*>/g, '').replace(/(.{40})/g, '$1\n')
			},
			{
				'type': '输入描述',
				'content': marked.parse(json.input).replace(/<[^>]*>/g, '').replace(/(.{40})/g, '$1\n')
			},
			{
				'type': '输出描述',
				'content': marked.parse(json.output).replace(/<[^>]*>/g, '').replace(/(.{40})/g, '$1\n')
			},
			{
				'type': '样例输入',
				'content': marked.parse(json.sample_input).replace(/<[^>]*>/g, '').replace(/(.{40})/g, '$1\n')
			},
			{
				'type': '样例输出',
				'content': marked.parse(json.sample_output).replace(/<[^>]*>/g, '').replace(/(.{40})/g, '$1\n')
			},
			{
				'type': '数据范围及提示',
				'content': marked.parse(json.hint).replace(/<[^>]*>/g, '').replace(/(.{40})/g, '$1\n')
			}
		], {head: 'def'});
		ct.drawTable([{
			'0': '操作',
			'1': '提交代码(S)',
			'2': '提交记录(R)',
			'3': '返回(B)'
		}], {head: false});
		print('>>>');
		input().then(function(res) {
			if (res == 'S' || res == 's') {
				
			} else if (res == 'R' || res == 'r') {
				
			} else if (res == 'B' || res == 'b') {
				problemList(token, 1, '');
			} else {
				problemSub(token, id);
			}
		});
	});
}

/* main */
init();
login();
