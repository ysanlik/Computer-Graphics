#version 410
//for gouraud
in vec4 color;
out vec4 fColor;

//for phong
in  vec3 fN;
in  vec3 fL;
in  vec3 fV;

uniform int PhongFlag;
uniform int GouraudFlag;
uniform int ModifiedFlag;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform float Shininess;
uniform vec4 Color;

in  vec2 texCoord;
uniform int TextureFlag;
uniform sampler2D tex; //texture object

void main()
{	
	if(PhongFlag==1){
		 // Normalize the input lighting vectors
        vec3 N = normalize(fN);
        vec3 V = normalize(fV);
        vec3 L = normalize(fL);

       
        vec3 R =normalize (reflect(L,N)); 

        vec4 ambient = AmbientProduct;

        float Kd = max(dot(L, N), 0.0);
        vec4 diffuse = Kd*DiffuseProduct;
        
        float Ks = pow(max(dot(V, R), 0.0), Shininess);
        vec4 specular = Ks*SpecularProduct;

        // discard the specular highlight if the light's behind the vertex
        if( dot(L, N) < 0.0 ) {
            specular = vec4(0.0, 0.0, 0.0, 1.0);
        }

        fColor = ambient + diffuse + specular;
		fColor += Color;
        fColor.a=1.0;

	}else if(GouraudFlag==1){
		fColor = color;
	}else if(ModifiedFlag==1){
		 // Normalize the input lighting vectors
        vec3 N = normalize(fN);
        vec3 V = normalize(fV);
        vec3 L = normalize(fL);

        vec3 H = normalize( L + V );
        
        vec4 ambient = AmbientProduct;

        float Kd = max(dot(L, N), 0.0);
        vec4 diffuse = Kd*DiffuseProduct;
        
        float Ks = pow(max(dot(N, H), 0.0), Shininess);
        vec4 specular = Ks*SpecularProduct;

        // discard the specular highlight if the light's behind the vertex
        if( dot(L, N) < 0.0 ) {
            specular = vec4(0.0, 0.0, 0.0, 1.0);
        }

        fColor = ambient + diffuse + specular;
		fColor += Color;
        fColor.a=1.0;
	}else if(TextureFlag==1){
	 //sample a texture color from texture object
        fColor = texture(tex, texCoord );
	}else{
		fColor = color;
	}
     
}

