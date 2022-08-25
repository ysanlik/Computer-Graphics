#version 410

in vec4 vPosition;
out vec4 color;

//for gouraud
in  vec3 vNormal;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

//for phong
// output values that will be interpretated per-fragment
out  vec3 fN;
out  vec3 fV;
out  vec3 fL;
uniform vec4 LightPosition;

in  vec2 vTexCoord;
out vec2 texCoord;

uniform int PhongFlag;
uniform int GouraudFlag;
uniform int ModifiedFlag;
uniform int TextureFlag;

uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 Color;

void main()
{	
	if(PhongFlag==1){
		// Transform vertex position into camera (eye) coordinates
		vec3 pos = (ModelView * vPosition).xyz;
    
		fN = (ModelView * vec4(vNormal, 0.0)).xyz; // normal direction in camera coordinates

		fV = -pos; //viewer direction in camera coordinates

		fL = LightPosition.xyz; // light direction if directional source

		if( LightPosition.w != 0.0 ) {
			fL = LightPosition.xyz - pos;  //if point light source
		}

	}else if(GouraudFlag==1){
		// Transform vertex position into camera (eye) coordinates
		vec3 pos = (ModelView * vPosition).xyz;
	
		vec3 L = normalize( LightPosition.xyz); //light direction
		vec3 V = normalize( -pos ); // viewer direction
		vec3 H = normalize( L + V ); // halfway vector

		// Transform vertex normal into camera coordinates
		vec3 N = normalize( ModelView * vec4(vNormal, 0.0) ).xyz;

		// Compute terms in the illumination equation
		vec4 ambient = AmbientProduct;

		float Kd = max( dot(L, N), 0.0 ); //set diffuse to 0 if light is behind the surface point
		vec4  diffuse = Kd*DiffuseProduct;

		float Ks = pow( max(dot(N, H), 0.0), Shininess );
		vec4  specular = Ks * SpecularProduct;
    
		//ignore also specular component if light is behind the surface point
		if( dot(L, N) < 0.0 ) {
			specular = vec4(0.0, 0.0, 0.0, 1.0);
		} 

		color = ambient + diffuse + specular;
		color+=Color;
		color.a = 1.0;
	}else if(ModifiedFlag==1){
			// Transform vertex position into camera (eye) coordinates
		vec3 pos = (ModelView * vPosition).xyz;
    
		fN = (ModelView * vec4(vNormal, 0.0)).xyz; // normal direction in camera coordinates

		fV = -pos; //viewer direction in camera coordinates

		fL = LightPosition.xyz; // light direction if directional source

		if( LightPosition.w != 0.0 ) {
			fL = LightPosition.xyz - pos;  //if point light source
		}
    
	}else if (TextureFlag==1){
		color = Color;
		texCoord= vTexCoord;
	}else{
		color = Color;
	}
     gl_Position = Projection * ModelView * vPosition;
}