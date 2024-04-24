#version 330

in vec3 a_Position;
in vec3 a_Velocity;
in float a_StartTime;
in float a_LifeTime;
in float a_Amp;
in float a_Period;
in float a_Value;
in vec4 a_Color;

out vec4 v_Color ;

uniform float u_Time = 0 ;
uniform float u_Period = 0.0f ;
uniform vec2 u_Acc = vec2(0 ,0 );
uniform vec2 u_AttractPos = vec2(0 ,0);

const vec3 c_StartPos = vec3(-1 , 0 , 0);
const vec3 c_Velocity = vec3(2.0, 0 , 0 );
const vec3 c_ParaVelocity = vec3(2.0, 2.0 , 0 );
const vec2 c_2DGravity = vec2(0.0,-4.9);
const float c_PI = 3.141592;

void Line(){

	float newTime = abs(fract(u_Time/u_Period) -0.5f)*2.0f;
	vec4 newPosition;
	newPosition.xyz = 
	(c_StartPos + a_Position) + c_Velocity * newTime;

	newPosition.w = 1.0f;
	gl_Position = newPosition;
	v_Color = a_Color;

}


void Velocity(){

	vec4 newPosition = vec4(a_Position , 1);
	float t = u_Time - a_StartTime;

	if(t>0){
	
	 t = a_LifeTime * fract(t/a_LifeTime);
	 float attractValue  = fract(t/a_LifeTime);
	 float tt = t*t;
	 
	newPosition.xy = newPosition.xy + a_Velocity.xy * t + 0.5 * (c_2DGravity + u_Acc)*tt;
	newPosition.xy = mix(newPosition.xy,u_AttractPos, attractValue);
	}
	else{
	newPosition.x = 10000;
	}
	gl_Position = newPosition;
	v_Color = a_Color;

}

void Circle(){

	float newTime = c_PI * 2.0f - fract(u_Time/u_Period) * c_PI * 2.0f;
	vec2 trans = vec2(cos(newTime) ,sin(newTime));
	vec4 newPosition;
	newPosition.xy = a_Position.xy + trans;
	newPosition.zw = vec2(0,1);
	gl_Position = newPosition;

	v_Color = a_Color;


}

void Parabola(){

float newTime = fract(u_Time/u_Period);
vec4 newPosition;
float t = newTime;
float tt = t*t;
float transX = (a_Position.x + c_StartPos.x) 
			+ c_ParaVelocity.x * newTime
			+0.5f *c_2DGravity.x * tt ;

float transY = (a_Position.y + c_StartPos.y) 
			+ c_ParaVelocity.y * newTime
			+0.5f *c_2DGravity.y * tt ;

newPosition.xy =vec2(transX, transY)	;
newPosition.zw  = vec2(0,1);
gl_Position = newPosition;
	v_Color = a_Color;

}

void Basic(){
vec4 newPosition = vec4(a_Position , 1);
gl_Position = newPosition; 
	v_Color = a_Color;

}

void SinShape(){


	vec4 newPosition = vec4(a_Position , 1);
	float t = u_Time - a_StartTime;
	
	float amp = a_Amp; 
	float period = a_Period;


	if(t>0){
	 t = a_LifeTime * fract(t/a_LifeTime);
	 float tt = t * t;
	 float value = a_Value * 2.0 * c_PI;
	 float x = cos(value);
	 float y=  sin(value);
	 newPosition.xy = newPosition.xy + vec2(x,y);

	 vec2 newVel = a_Velocity.xy + c_2DGravity * t;
	 vec2 newDir = vec2(-a_Velocity.y , a_Velocity.x);

	 newDir = normalize(newDir);
	newPosition.xy = newPosition.xy + a_Velocity.xy * t + 0.5f * c_2DGravity * tt;
	newPosition.xy = newPosition.xy + t*0.5 * amp * sin(period*t * c_PI);

	gl_Position = newPosition;

	}
	else{
	newPosition.x = 10000;
	gl_Position = newPosition;
	}	
	v_Color = a_Color;

}

void CircleShapeCycle(){


	vec4 newPosition = vec4(a_Position , 1);
	float t = u_Time - a_StartTime;
	
	float amp = a_Amp; 
	float period = a_Period;


	if(t>0){
	 t = a_LifeTime * fract(t/a_LifeTime);
	 float tt = t * t;
	 float value = a_StartTime * 2.0 * c_PI;
	 float x = cos(value);
	 float y=  sin(value);
	 newPosition.xy = newPosition.xy + vec2(x,y);

	 vec2 newVel = a_Velocity.xy + c_2DGravity * t;
	 vec2 newDir = vec2(-a_Velocity.y , a_Velocity.x);

	 newDir = normalize(newDir);
	newPosition.xy = newPosition.xy + a_Velocity.xy * t + 0.5f * c_2DGravity * tt;
	newPosition.xy = newPosition.xy + t*0.5 * amp * sin(period*t * c_PI);

	gl_Position = newPosition;

	}
	else{
	newPosition.x = 10000;
	gl_Position = newPosition;
	}	
	v_Color = a_Color;

}

void HeartShapeCycle(){


	vec4 newPosition = vec4(a_Position , 1);
	float t = u_Time - a_StartTime;
	
	float amp = a_Amp; 
	float period = a_Period;


	if(t>0){
	 t = a_LifeTime * fract(t/a_LifeTime);

	 float particleAlpha = 1 - t/ a_LifeTime;

	 float tt = t * t;
	 float value = a_StartTime * 2.0 * c_PI;
	 float x = 16 * pow(sin(value) ,3 );
	 float y=  13 * cos(value) -5 * cos(2*value) - 2 * cos(3*value) - cos (4* value);
	 x *=0.05f;
	 y *=0.05f;


	 newPosition.xy = newPosition.xy + vec2(x,y);

	 vec2 newVel = a_Velocity.xy + c_2DGravity * t;
	 vec2 newDir = vec2(-newVel.y , newVel.x);

	 newDir = normalize(newDir);
	newPosition.xy = newPosition.xy + a_Velocity.xy * t + 0.5f * c_2DGravity * tt;
	newPosition.xy = newPosition.xy + newDir *( t* 0.1) * amp * sin(period*t * c_PI);

	gl_Position = newPosition;
		v_Color = a_Color;
	v_Color.a = particleAlpha;
	}
	else{
	newPosition.x = 10000;
	gl_Position = newPosition;
	v_Color = a_Color;
	}	

}
void main()
{
//Line();
//Circle();
//Parabola();
//Basic();
Velocity();
//SinShape();
//CircleShapeCycle();

HeartShapeCycle();

}
