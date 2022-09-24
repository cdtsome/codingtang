/* require modules */
const fs = require('fs');
const os = require('os');
const cp = require('child_process');
const path = require('path');
const request = require('request');

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
		print(json);
	});
}

/* main */
init();
login();
