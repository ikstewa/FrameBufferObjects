#define KERNEL_SIZE 9
#define threshold 0.001

vec2 offset[KERNEL_SIZE];

uniform sampler2D colorMap;
uniform float width;
uniform float height;

// NVIDIA use const
float step_w = 1.0/width;
float step_h = 1.0/height;

vec4 col[KERNEL_SIZE];
float lum[KERNEL_SIZE];


void main()
{

	// set the offsets
	offset[0] = vec2(-step_w, -step_h);
	offset[1] = vec2(0.0, -step_h);
	offset[2] = vec2(step_w, -step_h);
	offset[3] = vec2(-step_w, 0.0);
	offset[4] = vec2(0.0, 0.0);
	offset[5] = vec2(step_w, 0.0);
	offset[6] = vec2(-step_w, step_h);
	offset[7] = vec2(0.0, step_h);
	offset[8] = vec2(step_w, step_h);
	

	vec3 rgb2lum = vec3(0.30, 0.59, 0.11);

	
	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		col[i] = texture2D(colorMap, gl_TexCoord[0].st + offset[i]);
		lum[i] = dot(col[i].rgb, rgb2lum);
//		lum[i] = col[i].r;
	}

	for (int i = 0; i < KERNEL_SIZE; i++)
	{
		lum[i] = col[i].r;
	}
	
	float x = lum[2] + lum[8] + 2*lum[5]-lum[0]-2*lum[3]-lum[6];
	float y = lum[6] +2*lum[7]+ lum[8]-lum[0]-2*lum[1]-lum[2];
	
	float edge = (x*x + y*y < threshold)? 1.0:0.0;
	//float edge = x*x + y*y;
	
//	gl_FragColor = vec4(edge, edge, edge, 1.0);
	gl_FragColor = vec4(lum[3], lum[3], lum[3], 1.0);
	
//	gl_FragColor = texture2D(colorMap, gl_TexCoord[0].st);

}
