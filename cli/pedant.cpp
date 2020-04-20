#include <ctime>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cctype>
using namespace std;

//#define DEBUG

namespace Pedant {
	// variables
	string task, task_file;
	vector<long long> rec;
	int ret_val = 0;
	struct Time;
	// functions
	bool read();
	void write();
	string time_to_string(long long, bool, bool);
	string length_to_string(long long len);
	void start();
	void stop();
	void view();
	void log();
	void add();
	void undo();
	void wrong_usage();
	long long input_time();
};

struct Pedant::Time{
	int year, month, day, hour, minute, second;
	Time(){}
	Time(long long llt){
		time_t tmt = (time_t)llt;
		tm *ltm = localtime(&tmt);
		year = ltm->tm_year + 1900;
		month = ltm->tm_mon + 1;
		day = ltm->tm_mday;
		hour = ltm->tm_hour;
		minute = ltm->tm_min;
		second = ltm->tm_sec;
	}
	Time(time_t tmt){
		tm *ltm = localtime(&tmt);
		year = ltm->tm_year + 1900;
		month = ltm->tm_mon + 1;
		day = ltm->tm_mday;
		hour = ltm->tm_hour;
		minute = ltm->tm_min;
		second = ltm->tm_sec;
	}
	long long to_long_long(){
		time_t now = time(0);
		tm *ltm = localtime(&now);
		ltm->tm_year = year - 1900;
		ltm->tm_mon = month - 1;
		ltm->tm_mday = day;
		ltm->tm_hour = hour;
		ltm->tm_min = minute;
		ltm->tm_sec = second;
		return (long long)mktime(ltm);
	}
	Time & add_year(int n){
		year += n;
		return (*this);
	}
	Time & add_month(int n){
		month += n;
		if(month > 12){
			year += (month - 1) / 12;
			month = (month - 1) % 12 + 1;
		}else if( month < 1){
			year += month / 12 - 1;
			month = month % 12 + 12;
		}
		return (*this);
	}
	Time & add_day(int n){
		long long llt = to_long_long() + 1LL*n*24*60*60;
		(*this) = Time(llt);
		return (*this);
	}
	Time & add_hour(int n){
		long long llt = to_long_long() + 1LL*n*60*60;
		(*this) = Time(llt);
		return (*this);
	}
	Time & add_minute(int n){
		long long llt = to_long_long() + 1LL*n*60;
		(*this) = Time(llt);
		return (*this);
	}
	Time & set_zero(){
		hour = minute = second = 0;
		return (*this);
	}
	bool same_year(const Time & b) const{
		return year == b.year;
	}
	bool same_month(const Time & b) const{
		return year == b.year && month == b.month;
	}
	bool same_day(const Time & b) const{
		return year == b.year && month == b.month && day == b.day;
	}
};
bool Pedant::read(){
	FILE *fp = fopen(task_file.c_str(), "r");
	if(fp == NULL)
		return false;
	long long llt;
	while(~fscanf(fp, "%lld", &llt))
		rec.emplace_back(llt);
	fclose(fp);
	return true;
}
void Pedant::write(){
	FILE *fp = fopen(task_file.c_str(), "w");
	for(int i = 0; i < rec.size(); ++i){
		if(i)fprintf(fp, "\n%lld", rec[i]);
		else fprintf(fp, "%lld", rec[i]);
	}
	fclose(fp);

	ret_val = 233;
}
string Pedant::time_to_string(long long llt, bool d, bool t){
	char buf[100];
	time_t tmt = (time_t)llt;
	tm *tmp = localtime(&tmt);
	if(d && t)
		sprintf(buf, "%d/%02d/%02d %02d:%02d:%02d", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday, tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	else if(d && !t)
		sprintf(buf, "%d/%02d/%02d", tmp->tm_year+1900, tmp->tm_mon+1, tmp->tm_mday);
	else if(!d && t)
		sprintf(buf, "%02d:%02d:%02d", tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	return string(buf);
}
string Pedant::length_to_string(long long len){
	char buf[100];
	if(len < 60)
		sprintf(buf, "%llds", len);
	else if(len < 60*60)
		sprintf(buf, "%lldm %llds", len/60, len%60);
	else
		sprintf(buf, "%lldh %lldm %llds", len/3600, (len%3600)/60, (len%3600)%60);
	return string(buf);
}
void Pedant::start(){
	read();
	if(rec.size() % 2 == 1)
		printf("Error: Task \"%s\" is in progress since\n\t%s\nStop it to start again !\n", task.c_str(), time_to_string(rec.back(), 1, 1).c_str());
	else{
		rec.emplace_back((long long)time(0));
		write();
		printf("Successfully start task \"%s\" at\n\t%s\n", task.c_str(), time_to_string(rec.back(), 1, 1).c_str());
	}
}
void Pedant::stop(){
	if(!read())
		printf("Error: Task \"%s\" has not been created !\n", task.c_str());
	else if(rec.size() == 0)
		printf("Error: Task \"%s\" is not in progress !\n", task.c_str());
	else if(rec.size() % 2 == 0)
		printf("Error: Task \"%s\" is not in progress since\n\t%s\n", task.c_str(), time_to_string(rec.back(), 1, 1).c_str());
	else{
		long long sta = rec.back(), sto = (long long)time(0);
		rec.emplace_back(sto);
		write();
		printf("Successfully stop task \"%s\" at\n\t%s\nExecution time: %s\n", task.c_str(), time_to_string(sto, 1, 1).c_str(), length_to_string(sto - sta).c_str());
	}
}
void Pedant::view(){
	if(!read())
		printf("Error: Task \"%s\" has not been created !\n", task.c_str());
	else if(rec.empty())
		printf("Error: Task \"%s\" has no record !\n", task.c_str());
	else{
		vector<pair<Time, long long> > days;
		vector<long long> years, months;
		int ndays;
		long long tot_length, ave_length;

		if(rec.size() % 2)
			rec.emplace_back((long long)time(0));
		for(int i = 0; i < rec.size(); i += 2){
			long long a = rec[i], b = rec[i+1];
			while(a < b){
				Time t(a);
				t.set_zero();
				if(days.empty() || !days.back().first.same_day(t))
					days.emplace_back(t, 0);
				long long next = t.to_long_long() + 24*60*60;
				days.back().second += min(b, next) - a;
				a = next;
			}
		}
#ifdef DEBUG
		printf("here\n");
#endif
		long long cy = 0, cm = 0;
		for(int i = 0; i < days.size(); ++i){
			cy += days[i].second;
			cm += days[i].second;
			tot_length += days[i].second;
			if(i == days.size() - 1 || !days[i].first.same_year(days[i+1].first)){
				years.emplace_back(cy);
				cy = 0;
			}else
				years.emplace_back(-1);
			if(i == days.size() - 1 || !days[i].first.same_month(days[i+1].first)){
				months.emplace_back(cm);
				cm = 0;
			}else
				months.emplace_back(-1);
		}
#ifdef DEBUG
		printf("here\n");
#endif
		ndays = (days.back().first.to_long_long() - days.front().first.to_long_long()) / (24*60*60);
		ave_length = tot_length / ndays;
#ifdef DEBUG
		printf("here\n");
#endif

		//task name
		printf("\033[4mTask\033[0m: %s\n", task.c_str());
		//current status
		printf("\n\033[4mCurrent status\033[0m: ");
		if(rec.size() % 2 == 1)
			printf("running (since %s)\n", time_to_string(rec.back(), 1, 1).c_str());
		else if(rec.size() == 0)
			printf("not running\n\n");
		else
			printf("not running (since %s)\n", time_to_string(rec.back(), 1, 1).c_str());
		//today
		Time today(time(0));
		today.set_zero();
		printf("\n\033[4mToday\033[0m: ");
		if(!today.same_day(days.back().first)){
			printf("0s\n");
		}else{
			printf("%s\n", length_to_string(days.back().second).c_str());
			long long tmp = today.to_long_long();
			for(int i = rec.size() - 1; i >= 0; i -= 2){
				if(i % 2 == 0){
					printf("%s -\n", time_to_string(max(tmp, rec[i]), 0, 1).c_str());
					i++;
				}else if(rec[i] > tmp){
					printf("%s - %s\n", time_to_string(max(tmp, rec[i-1]), 0, 1).c_str(), time_to_string(rec[i], 0, 1).c_str());
				}else
					break;
			}
		}
		//total
		printf("\nYou've start recording for about %d days. In total, you've spent %s on this task, about %s a day.\n", ndays, length_to_string(tot_length).c_str(), length_to_string(ave_length).c_str());
		//history
		for(int i = days.size() - 1; i >= 0; --i){
			if(years[i] != -1)
				printf("\033[31m%d\033[0m: %s\n", days[i].first.year, length_to_string(years[i]).c_str());
			if(months[i] != -1)
				printf("    \033[32m%2d\033[0m: %s\n", days[i].first.month, length_to_string(months[i]).c_str());
			printf("      \033[33m%2d\033[0m: %s\n", days[i].first.day, length_to_string(days[i].second).c_str());
		}
	}
}
void Pedant::log(){
	if(!read())
		printf("Error: Task \"%s\" has not been created !\n", task.c_str());
	else for(int i = 0; i < rec.size(); i += 2){
		printf("%3d: ", i/2+1);
		if(i != rec.size() - 1)
			printf("%s - %s\n", time_to_string(rec[i], 1, 1).c_str(), time_to_string(rec[i+1], 1, 1).c_str());
		else
			printf("%s -\n", time_to_string(rec[i], 1, 1).c_str());
	}
}
void Pedant::add(){
	read();
	if(rec.size()%2 == 1){
		printf("STOP: ");
		long long llt = input_time();
#ifdef DEBUG
		printf("Input Stop: %lld\n", llt);
#endif
		if(rec.back() >= llt)
			printf("Error: Stop time <= start time !\n");
		else if(llt > (long long)time(0))
			printf("Error: Stop time > now !\n");
		else{
			rec.emplace_back(llt);
			write();
		}
	}else{
		long long llt[2];
		printf("START: ");
		llt[0] = input_time();
#ifdef DEBUG
		printf("Input Start: %lld\n", llt[0]);
#endif
		if(llt[0] == 0){
			printf("Error: Start time cannot be empty !\n");
			return;
		}
		printf("\nSTOP: ");
		llt[1] = input_time();
#ifdef DEBUG
		printf("Input Stop: %lld\n", llt[1]);
#endif
		if(llt[1] == 0){
			if(!rec.empty() && llt[0] < rec.back())
				printf("Error: Overlap !\n");
			else{
				rec.emplace_back(llt[0]);
				write();
			}
		}else if(llt[0] >= llt[1])
			printf("Error: Start time >= stop time !\n");
		else if(llt[1] > (long long)time(0))
			printf("Error: Stop time > now !\n");
		else{
			int p = rec.size() - 1;
			while(p >= 1 && rec[p] > llt[0])
				p -= 2;
			if(p+1 < rec.size() && llt[1] > rec[p+1])
				printf("Error: Overlap !\n");
			else{
				rec.insert(rec.begin()+p+1, llt[1]);
				rec.insert(rec.begin()+p+1, llt[0]);
				write();
				printf("Add succeed !\n");
			}
		}
	}
}
void Pedant::undo(){
	if(!read())
		printf("Error: Task \"%s\" has not been created !\n", task.c_str());
	else if(rec.empty())
		printf("Error: Task \"%s\" has no record !\n", task.c_str());
	else{
		long long llt = rec.back();
		rec.pop_back();
		write();
		printf("Successfully undo task \"%s\"\n\t%s ", task.c_str(), time_to_string(llt, 1, 1).c_str());
		if(rec.size() % 2)
			printf("STOP\n");
		else
			printf("START\n");
	}
}
void Pedant::wrong_usage(){
	printf("Usage : pedant [start/stop/view/log/add/undo] [task]\n");
}
long long Pedant::input_time(){
	//get current year, month, date...
	time_t tmt = time(0);
	long long now = (long long)tmt;
	tm *ltm = localtime(&tmt);
	int year = ltm->tm_year + 1900;
	int month = ltm->tm_mon + 1;
	int day = ltm->tm_mday;

	vector<int> vt;
	pair<int, int> range[5] = {
		{1900, 3000},
		{1, 12},
		{1, 31},
		{0, 23},
		{0, 59}
	};
	char str[10];
	while(vt.size() < 5){
		int p = vt.size();
		scanf("%s", str);
		if(strcmp(str, "x") == 0)
			return 0;
		//analyze str[]
		int n = 0;
		int len = strlen(str);
		int c_a = 0, c_n = 0, c_m = 0;
		for(char * ch = str; *ch; ch++){
			if(isalpha(*ch))
				c_a++;
			else if(isdigit(*ch)){
				c_n++;
				n = n * 10 + int((*ch) - '0');
			}else if((*ch) == '-')
				c_m++;
		}
		//
		if(c_n == len){ //it is a number
			if(n >= range[p].first && n <= range[p].second)
				vt.emplace_back(n);
			else
				printf("Error: Invalid number !\n");
		}else if(isalpha(str[0]) && c_a == 1 && ((c_a + c_m == len) || (c_m == 1 && str[1] == '-' && c_a + c_n + c_m == len))){
			if(vt.size()){
				printf("Error: Invalid symbol !\n");
				continue;
			}
			if(c_n)
				n = -n;
			else
				n = -c_m;
			if(str[0] == 'y')
				vt.emplace_back(Time(time(0)).add_year(n).year);
			else if(str[0] == 'm'){
				Time t = Time(time(0)).add_month(n);
				vt.emplace_back(t.year);
				vt.emplace_back(t.month);
			}else if(str[0] == 'd'){
				Time t = Time(time(0)).add_day(n);
				vt.emplace_back(t.year);
				vt.emplace_back(t.month);
				vt.emplace_back(t.day);
			}else if(str[0] == 'h'){
				Time t = Time(time(0)).add_hour(n);
				vt.emplace_back(t.year);
				vt.emplace_back(t.month);
				vt.emplace_back(t.day);
				vt.emplace_back(t.hour);
			}else if(str[0] == 'n'){
				Time t = Time(time(0)).add_minute(n);
				vt.emplace_back(t.year);
				vt.emplace_back(t.month);
				vt.emplace_back(t.day);
				vt.emplace_back(t.hour);
				vt.emplace_back(t.minute);
			}else
				printf("Error: Invalid symbol !\n");
		}else
			printf("Error: Invalid symbol !\n");
	}
	printf("%d/%02d/%02d %02d:%02d\n", vt[0], vt[1], vt[2], vt[3], vt[4]);
	Time t;
	t.year = vt[0], t.month = vt[1], t.day = vt[2], t.hour = vt[3], t.minute = vt[4], t.second = 0;
	return t.to_long_long();
}

int main(int argc, char ** argv){
	if(argc != 3)
		Pedant::wrong_usage();
	else{
		Pedant::task = argv[2];
		Pedant::task_file = Pedant::task + ".pdt";

		printf("\n");

		if(strcmp(argv[1], "start") == 0)
			Pedant::start();
		else if(strcmp(argv[1], "stop") == 0)
			Pedant::stop();
		else if(strcmp(argv[1], "view") == 0)
			Pedant::view();
		else if(strcmp(argv[1], "log") == 0)
			Pedant::log();
		else if(strcmp(argv[1], "add") == 0)
			Pedant::add();
		else if(strcmp(argv[1], "undo") == 0)
			Pedant::undo();
		else
			Pedant::wrong_usage();

		printf("\n");
	}

	return Pedant::ret_val;
}
