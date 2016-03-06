#ifndef GEN_NUM_H_
#define GEN_NUM_H_

struct rectangle_t;

int min(int a, int b);
int min3(int a, int b, int c);
int min4(int a, int b, int c, int d);
int min5(int a, int b, int c, int d, int e);
int min6(int a, int b, int c, int d, int e, int f);
int max(int a, int b);
int max3(int a, int b, int c);
int max4(int a, int b, int c, int d);
int max5(int a, int b, int c, int d, int e);
int max6(int a, int b, int c, int d, int e, int f);

int rand_up_to(int max);
int rand_two_ways(int max);
int rand_avg(int max);
int gcd(int u, int v);

int distance(int x1, int y1, int x2, int y2);

double rads_from_degrees(int degrees);
int degrees_from_rads(double rads);
void angle_to_normal_vect(int degrees, double *x, double *y);
void angle_to_vect(int degrees, int mag, int *x, int *y);
int angle_from_vect(int x, int y);
void vect_at_least(int degrees, int speed, int minx, int miny, int *out_dur,
                   int *out_x, int *out_y);
int degrees_normalize(int degrees);
int angle_distance_clockwise(int from, int to);

int min_ints(int num_ints, ...);
int max_ints(int num_ints, ...);

double circle_distance_ratio(int radius, int dist, double distance_multiplier);

int div_floor(int n, int d);

int simple_strtoul(unsigned long *val, char *src, int base);

#endif

