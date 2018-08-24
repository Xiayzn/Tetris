#include <stdio.h>
#include <tetris.h>
#include <def.h>
#include <keyboard.h>
#include <stdlib.h>
#include <string.h>

void draw_element(int x, int y, int c)
{
	x *= 2;
	x++;
	y++;

	printf("\033[%d;%dH", y, x);
	printf("\033[3%dm\033[4%dm", c, c);
	printf("[]");
	fflush(stdout);
	printf("\033[?25l");
	printf("\033[0m");
}

void draw_shape(int x, int y, struct shape p, int c)
{
	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			if ( p.s[i][j] != 0 ) {
				draw_element(x+j, y+i, c);
			}
		}
	}
}

int over( void )
{
	for (int i=0; i<W; i++) {
		if ( background[1][i] != 0 ) {
			recover_keyboard();
			printf("\033[?25h");
			exit(0);
		}
	}
	return 1;
}

int can_move(int x, int y, struct shape p)
{
	for (int i=0; i<5; i++){
		for (int j=0; j<5; j++) {
			if ( p.s[i][j] == 0 )
				continue;
			if ( x+j >= W )
				return 0;
			if ( x+j < 0 )
				return 0;
			if ( y+i >= H )
				return 0;
			if ( background[y+i][x+j] != 0) 
				return 0;
		}
	}

	return 1;
}

void draw_back( void )
{
	for (int i=0; i<H; i++) {
		for (int j=0; j<W; j++) {
			if ( background[i][j] != 0 ) {
				draw_element(j, i, FC);
			} else {
				draw_element(j, i, BC);
			}
		}
	}
}

void set_back(struct data *p, struct shape shp)
{
	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			if ( shp.s[i][j] != 0 ) {
				background[p->y+i][p->x+j] = 1;
			}
		}
	}
}

void clean_line( void )
{
	for (int i=0; i<H; i++) {
		int t = 0;
		for (int j=0; j<W; j++) {
			if ( background[i][j] != 0 )
				t++;
		}
		if ( t == W ) {
			for (int k=i; k>0; k-- ) {
				memcpy(background[k], background[k-1], sizeof background[k]);
			}
		}
	}
}

void show_next( void )
{
	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			if ( shape_arr[next].s[i][j] == 0 )
				draw_element(W+1+j, 0+i, BC);
			else
				draw_element(W+1+j, 0+i, FC);
		}
	}
}

void timer_tetris(struct data *p)
{
	if ( can_move(p->x, p->y+1, shape_arr[cur]) ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		p->y++;
		draw_shape(p->x, p->y, shape_arr[cur], FC);
	} else {
		set_back(p, shape_arr[cur]);
		clean_line();
		over();
		draw_back();
		p->x = 0;
		p->y = 0;
		cur = next;
		next = rand() % 7;
		show_next();
	}
}

static struct shape turn_90(struct shape p)
{
	struct shape t;

	for (int i=0; i<5; i++) {
		for (int j=0; j<5; j++) {
			t.s[i][j] = p.s[4-j][i];
		}
	}
	
	return t;
}

void tetris(struct data *p)
{
	int c = get_key();

	if ( is_left(c) ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		if ( can_move(p->x-1, p->y, shape_arr[cur]) ) {
			p->x--;
		}
		draw_shape(p->x, p->y, shape_arr[cur], FC);
	}
	if ( is_right(c) ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		if ( can_move(p->x+1, p->y, shape_arr[cur]) ) {
			p->x++;
		}
		draw_shape(p->x, p->y, shape_arr[cur], FC);
	}
	if ( is_up(c) ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		shape_arr[cur] = turn_90(shape_arr[cur]);
		if ( 0 == can_move(p->x, p->y, shape_arr[cur]) ) {
			shape_arr[cur] = turn_90(shape_arr[cur]);
			shape_arr[cur] = turn_90(shape_arr[cur]);
			shape_arr[cur] = turn_90(shape_arr[cur]);
		}
		draw_shape(p->x, p->y, shape_arr[cur], FC);
	}
	if ( is_down(c) ) {
		draw_shape(p->x, p->y, shape_arr[cur], BC);
		if ( can_move(p->x, p->y+1, shape_arr[cur]) ) {
			p->y++;
		}
		draw_shape(p->x, p->y, shape_arr[cur], FC);
	}
	if ( is_esc(c) ) {
		printf("\033[?25h");
		recover_keyboard();
		exit(0);
	}
}

