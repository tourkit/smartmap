vec4 colorize(vec4 rgba, float red, float green, float blue) {

    return rgba * vec4 (red,green,blue,1);

}