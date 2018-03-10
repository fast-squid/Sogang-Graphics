#version 330

struct LIGHT {
	vec4 position; // assume point or direction in EC in this example shader
	vec4 ambient_color, diffuse_color, specular_color;
	vec4 light_attenuation_factors; // compute this effect only if .w != 0.0f
	vec3 spot_direction;
	float spot_exponent;
	float blind_width;
	float spot_cutoff_angle;
	bool light_on;
};

struct MATERIAL {
	vec4 ambient_color;
	vec4 diffuse_color;
	vec4 specular_color;
	vec4 emissive_color;
	float specular_exponent;
};

uniform vec4 u_global_ambient_color;
#define NUMBER_OF_LIGHTS_SUPPORTED 4
uniform LIGHT u_light[NUMBER_OF_LIGHTS_SUPPORTED];
uniform MATERIAL u_material;

uniform bool u_blind_effect;
uniform int u_shading_mode;
uniform int u_win_flag;
uniform int u_cir_flag;
uniform int u_club_flag;
uniform float u_light_strength=1;
const float zero_f = 0.0f;
const float one_f = 1.0f;
float win_width=25;

in vec3 v_position_EC;
in vec3 v_normal_EC;
layout (location = 0) out vec4 final_color;
in vec3 a_pos_WC;

vec4 lighting_equation(in vec3 P_EC, in vec3 N_EC) {
	vec4 color_sum;
	float local_scale_factor, tmp_float,tmp_float2,tmp_float3; 
	vec3 L_EC;
	vec3 tmp_li1=vec3(1,0,0);
	vec3 tmp_li2=vec3(0,0,1);

	color_sum = u_material.emissive_color + u_global_ambient_color * u_material.ambient_color;
 
	for (int i = 0; i < NUMBER_OF_LIGHTS_SUPPORTED; i++) {
		if (!u_light[i].light_on) continue;

		local_scale_factor = one_f;
		if (u_light[i].position.w != zero_f) { // point light source
			L_EC = u_light[i].position.xyz - P_EC.xyz;

			if (u_light[i].light_attenuation_factors.w  != zero_f) {
				vec4 tmp_vec4;

				tmp_vec4.x = one_f;
				tmp_vec4.z = dot(L_EC, L_EC);
				tmp_vec4.y = sqrt(tmp_vec4.z);
				tmp_vec4.w = zero_f;
				local_scale_factor = one_f/dot(tmp_vec4, u_light[i].light_attenuation_factors);
			}

			L_EC = normalize(L_EC);

			if (u_light[i].spot_cutoff_angle < 180.0f) { // [0.0f, 90.0f] or 180.0f
				float spot_cutoff_angle = clamp(u_light[i].spot_cutoff_angle, zero_f, 90.0f);
				vec3 spot_dir = normalize(u_light[i].spot_direction);

				tmp_float = dot(-L_EC, spot_dir);
				tmp_float2 = dot(-L_EC,tmp_li1);
				tmp_float3 = dot(-L_EC,tmp_li2);

				if (tmp_float >= cos(radians(u_light[i].spot_cutoff_angle))) {
					if (!u_blind_effect) {
						// Normal Phong shading mode
			 			tmp_float = pow(tmp_float, u_light[i].spot_exponent);
					}
					else {
						// When you read this shader first time, just ignore this blind effect!!!
						if(i==1){
							tmp_float = cos(180.0f*acos(tmp_float)*u_light[i].blind_width+u_club_flag);
							if (tmp_float < zero_f) 
								tmp_float = 0;
						}
						if(i==2){
							tmp_float = tan(180*atan(tmp_float)*u_light[i].blind_width+u_club_flag);
							if (tmp_float < zero_f) 
								tmp_float = 0;
						}
						if(i==3){
							tmp_float2 = cos(180.0f*acos(tmp_float2)*u_light[i].blind_width+u_club_flag);
							tmp_float3 = cos(180.0f*acos(tmp_float3)*u_light[i].blind_width+u_club_flag);
							tmp_float = tmp_float2+tmp_float3;
							tmp_float = pow(tmp_float, u_light[i].spot_exponent);
							if (tmp_float3 < zero_f&&tmp_float2<zero_f) 
								tmp_float = 0;
						}
					}
				}
				else 
					tmp_float = zero_f;
				local_scale_factor *= tmp_float;
			}
		}
		else {  // directional light source
			L_EC = normalize(u_light[i].position.xyz);
		}	

		if (local_scale_factor > zero_f) {				
			vec4 local_color_sum = u_light[i].ambient_color * u_material.ambient_color;

			tmp_float = max(zero_f, dot(N_EC, L_EC));  
			local_color_sum += u_light[i].diffuse_color*u_material.diffuse_color*tmp_float;
			
			vec3 H_EC = normalize(L_EC - normalize(P_EC));
			tmp_float = max(zero_f, dot(N_EC, H_EC)); 
			if (tmp_float> zero_f) {
				local_color_sum += u_light[i].specular_color
				                       *u_material.specular_color*pow(tmp_float,u_material.specular_exponent);
			}
			color_sum += local_scale_factor*local_color_sum;
			if(i!=0) color_sum*=u_light_strength;
		}
	}
	
 	return color_sum;
}

void main(void) {
    vec3 tmp_vert;
	float res=(250.0f/u_win_flag);		
	float res1=(40.0f/u_cir_flag);
	vec3 vx,vy,vz;
	vx=vec3(1,0,0); 
	vy=vec3(0,1,0);
	vz=vec3(0,0,1);

	if(u_shading_mode==1) final_color = vec4(normalize(v_normal_EC),0);
	else final_color = lighting_equation(v_position_EC, normalize(v_normal_EC));

	tmp_vert=a_pos_WC;
	if(u_win_flag!=0){
		
		while(tmp_vert.y>res){
			tmp_vert.y=tmp_vert.y-res;
		}
		while(tmp_vert.z>res){
			tmp_vert.z=tmp_vert.z-res;
		}
		if(u_win_flag!=1){
			if(tmp_vert.y<res*0.1 || tmp_vert.z<res*0.1|| tmp_vert.y > res*0.9
			|| tmp_vert.z>res*0.9)	
			discard;
		}
	}
	if(u_cir_flag!=0){
		while(tmp_vert.x>res1){
			tmp_vert.x=tmp_vert.x-res1;
		}
		while(tmp_vert.y>res1){
			tmp_vert.y=tmp_vert.y-res1;
		}
		while(tmp_vert.z>res1){
			tmp_vert.z=tmp_vert.z-res1;
		}
		normalize(tmp_vert);
		if(dot(tmp_vert,vx)<0.3||dot(tmp_vert,vx)>0.7) discard;
		if(dot(tmp_vert,vy)<0.3||dot(tmp_vert,vy)>0.7) discard;
		if(dot(tmp_vert,vz)<0.3||dot(tmp_vert,vz)>0.7) discard;	
	}
	
}
