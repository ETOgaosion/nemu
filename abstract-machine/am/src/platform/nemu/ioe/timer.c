#include <am.h>
#include <nemu.h>
#include <klib.h>

uint32_t boot_time_hi;
uint32_t boot_time_lo;

#define DAYS_PER_400Y (365*400 + 97)
#define DAYS_PER_100Y (365*100 + 24)
#define DAYS_PER_4Y   (365*4   + 1)

#define INT_MIN	(-INT_MAX - 1)
#define INT_MAX	2147483647

struct tm {
  int tm_sec; /* 秒 – 取值区间为[0,59] */
  int tm_min; /* 分 - 取值区间为[0,59] */
  int tm_hour; /* 时 - 取值区间为[0,23] */
  int tm_mday; /* 一个月中的日期 - 取值区间为[1,31] */
  int tm_mon; /* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
  int tm_year; /* 年份，其值等于实际年份减去1900 */
  int tm_wday; /* 星期 – 取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
  int tm_yday; /* 从每年的1月1日开始的天数 – 取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
  int tm_isdst; /* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的时候，tm_isdst为0；不了解情况时，tm_isdst()为负。*/
  long int tm_gmtoff; /*指定了日期变更线东面时区中UTC东部时区正秒数或UTC西部时区的负秒数*/
  const char *tm_zone; /*当前时区的名字(与环境变量TZ有关)*/
};

int __secs_to_tm(long long t, struct tm *tm)
{
	long long days, secs;
	int remdays, remsecs, remyears;
	int qc_cycles, c_cycles, q_cycles;
	int years, months;
	int wday, yday;
	static char days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};

	/* Reject time_t values whose year would overflow int */
	if (t < INT_MIN * 31622400LL || t > INT_MAX * 31622400LL)
		return -1;

	secs = t;
	days = secs / 86400;
	remsecs = secs % 86400;

	wday = (4 + days) % 7;

	qc_cycles = days / DAYS_PER_400Y;
	remdays = days % DAYS_PER_400Y;

	c_cycles = remdays / DAYS_PER_100Y;
	remdays -= c_cycles * DAYS_PER_100Y;

	q_cycles = remdays / DAYS_PER_4Y;
	remdays -= q_cycles * DAYS_PER_4Y;

	remyears = remdays / 365;
	remdays -= remyears * 365;

  yday = remdays;

	years = remyears + 4 * q_cycles + 100 * c_cycles + 400 * qc_cycles;

  if (remyears == 2) {
    days_in_month[1]++;
  }

	for (months=0; days_in_month[months] <= remdays; months++) {
		remdays -= days_in_month[months];
  }

	tm->tm_year = years + 70;
	tm->tm_mon = months;
	tm->tm_mday = remdays + 1;
	tm->tm_wday = wday;
	tm->tm_yday = yday;

	tm->tm_hour = remsecs / 3600 + 8;
	tm->tm_min = remsecs / 60 % 60;
	tm->tm_sec = remsecs % 60;

	return 0;
}

void __am_timer_init() {
  boot_time_lo = inl(RTC_ADDR);
  boot_time_hi = inl(RTC_ADDR + 4);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  uint32_t time_lo = inl(RTC_ADDR) - boot_time_lo;
  uint32_t time_hi = inl(RTC_ADDR + 4) - boot_time_hi;
  uint64_t res = ((uint64_t)time_hi << 32) + time_lo;
  uptime->us = res;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  uint32_t time_lo = inl(RTC_ADDR + 8);
  uint32_t time_hi = inl(RTC_ADDR + 12);
  uint64_t time = ((uint64_t)time_hi << 32) + time_lo;
  struct tm tm_time;
  __secs_to_tm(time, &tm_time);
  rtc->second = tm_time.tm_sec;
  rtc->minute = tm_time.tm_min;
  rtc->hour   = tm_time.tm_hour;
  rtc->day    = tm_time.tm_mday;
  rtc->month  = tm_time.tm_mon + 1;
  rtc->year   = tm_time.tm_year + 1900;
}
