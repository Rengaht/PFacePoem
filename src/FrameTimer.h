#ifndef FRAME_TIMER_H
#define FRAME_TIMER_H

#include "ofMain.h"

class FrameTimer{
public:

	ofEvent<int> finish_event;

	FrameTimer(){
		setup(0,0);
	}
	FrameTimer(float len_){
		setup(len_,0);
	}
	FrameTimer(float len_,float delay_){
		setup(len_,delay_);
	}
	void start(){
		started=true;
	}
	void stop(){
		started=false;
	}
	void reset(){
		started=false;
		ani_t=-delay;
		event_raised=false;
		num_cycle=0;
	}
	void update(float dt_){
		if(!started) return;
		if(ani_t<due) ani_t+=dt_;
		if(ani_t>=due){

			if(!continuous && event_raised) return;

			event_raised=true;
			num_cycle++;

			int data=num_cycle;
			ofNotifyEvent(finish_event,data);

			if(continuous) ani_t=0;

		}
	}
	float val(){
		//if(!started) return 0;
		if(ani_t<0) return 0;
		if(ani_t>=due) return 1;

		return ofClamp(ani_t/due,0,1);
	}
	float eval(){
		float v=val();
		return v*v;
	}
	int count(){
		if(ani_t<0) return ceil(abs(ani_t/1000.0));
		else return floor((due-ani_t)/1000.0);
	}
	void restart(){
		reset();
		start();
	}
	int num(){
		return num_cycle;
	}
	void setContinuous(bool cont){
		continuous=cont;
	}
    void setDue(float set_){
        ani_t=ani_t/due*set_;
        due=set_;
    }
    float getDue(){
        return due;
    }
	float valCubic(){
		float p=val();
		return valCubic(p);
	}
	float valCubic(float p){
		p/=.5;
		if(p<1) return .5*p*p*p;
		p-=2;
		return .5*(p*p*p+2);
	}
	float valEase(){
		float p=val();
		return sin(p *HALF_PI);
	}
	float valFade(){
		float d_=.5;
		float p=val();
		if(p<d_){
			return valCubic(p/.5);
		}else{
			p-=.5;
			return 1.0-valCubic(p/.5);
		}
	}
	bool isStart(){
		return started;
	}

	bool finish(){
		return ani_t>=due;
	}
private:
	float ani_t;
	float due,delay;
	bool started;
	bool event_raised;
	//bool loop;
	int num_cycle;
	bool continuous;

	void setup(float len_,float delay_){
		due=len_;
		delay=delay_;
		continuous=false;
		reset();
	}


};

#endif
