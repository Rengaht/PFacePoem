
precision highp float;

uniform sampler2D tex0;

uniform float blurAmnt;
uniform float windowWidth;
uniform float windowHeight;

varying vec2 texCoordVarying;

// Gaussian weights from http://dev.theomader.com/gaussian-kernel-calculator/

void main()
{
//	float blur=1.0;
//	vec2 dir=vec2(blur/windowWidth,blur/windowHeight);
//	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
//
//	color+=texture2D(tex0,texCoordVarying+vec2(-dir.x,0.0));
//	color+=texture2D(tex0,texCoordVarying+vec2(dir.x,0.0));
//
//	color+=texture2D(tex0,texCoordVarying+vec2(0.0,-dir.y));
//	color+=texture2D(tex0,texCoordVarying+vec2(-dir.x,-dir.y));
//	color+=texture2D(tex0,texCoordVarying+vec2(dir.x,-dir.y));
//
//	color+=texture2D(tex0,texCoordVarying+vec2(0.0,dir.y));
//	color+=texture2D(tex0,texCoordVarying+vec2(-dir.x,dir.y));
//	color+=texture2D(tex0,texCoordVarying+vec2(-dir.x,dir.y));
//
//	color*=0.111111;
//
//	color+=texture2D(tex0,texCoordVarying);

	vec4 color = vec4(0.0);
	
	vec2 resolution=vec2(windowWidth,windowHeight);

	for(int i=0;i<2;++i){

	vec2 direction=vec2(1.0,0.0);
	if(i==1) direction=vec2(0.0,1.0);

	vec2 off1 = vec2(1.411764705882353) * direction;
	vec2 off2 = vec2(3.2941176470588234) * direction;
 	vec2 off3 = vec2(5.176470588235294) * direction;
  	
	color += texture2D(tex0, texCoordVarying) * 0.1964825501511404;	
  	color += texture2D(tex0, texCoordVarying + (off1 / resolution)) * 0.2969069646728344;
	color += texture2D(tex0, texCoordVarying - (off1 / resolution)) * 0.2969069646728344;
	color += texture2D(tex0, texCoordVarying + (off2 / resolution)) * 0.09447039785044732;
	color += texture2D(tex0, texCoordVarying - (off2 / resolution)) * 0.09447039785044732;
	color += texture2D(tex0, texCoordVarying + (off3 / resolution)) * 0.010381362401148057;
	color += texture2D(tex0, texCoordVarying - (off3 / resolution)) * 0.010381362401148057;
	}


	gl_FragColor = vec4(color.rgb,1.0);
}
