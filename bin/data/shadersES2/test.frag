
precision highp float;

uniform sampler2D tex0;

uniform float blurAmnt;
uniform float windowWidth;
uniform float windowHeight;

varying vec2 texCoordVarying;

// Gaussian weights from http://dev.theomader.com/gaussian-kernel-calculator/

void main()
{
	float blur=20.0;
	vec2 dir=vec2(blur/windowWidth,blur/windowHeight);
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);

	color += 0.000229 * texture2D(tex0, texCoordVarying + vec2(blurAmnt * -4.0, 0.0));
	color += 0.005977 * texture2D(tex0, texCoordVarying + vec2(blurAmnt * -3.0, 0.0));
	color += 0.060598 * texture2D(tex0, texCoordVarying + vec2(blurAmnt * -2.0, 0.0));
	color += 0.241732 * texture2D(tex0, texCoordVarying + vec2(blurAmnt * -1.0, 0.0));

	color += 0.382928 * texture2D(tex0, texCoordVarying + vec2(0.0, 0));

	color += 0.241732 * texture2D(tex0, texCoordVarying + vec2(blurAmnt * 1.0, 0.0));
	color += 0.060598 * texture2D(tex0, texCoordVarying + vec2(blurAmnt * 2.0, 0.0));
	color += 0.005977 * texture2D(tex0, texCoordVarying + vec2(blurAmnt * 3.0, 0.0));
	color += 0.000229 * texture2D(tex0, texCoordVarying + vec2(blurAmnt * 4.0, 0.0));
    
	//color+=texture2D(tex0,texCoordVarying+vec2(-dir.x,0.0));
	//color+=texture2D(tex0,texCoordVarying+vec2(dir.x,0.0));

	//color+=texture2D(tex0,texCoordVarying+vec2(0.0,-dir.y));
	//color+=texture2D(tex0,texCoordVarying+vec2(-dir.x,-dir.y));
	//color+=texture2D(tex0,texCoordVarying+vec2(dir.x,-dir.y));

	//color+=texture2D(tex0,texCoordVarying+vec2(0.0,dir.y));
	//color+=texture2D(tex0,texCoordVarying+vec2(-dir.x,dir.y));
	//color+=texture2D(tex0,texCoordVarying+vec2(-dir.x,dir.y));

	//color*=0.111111;


    gl_FragColor = color;
}
