Shader "t1" {
  SubShader {
    Tags {"Queue" = "Geometry +11" } // earlier = hides stuff later in queue
    Lighting Off
    ZTest LEqual
    ZWrite On
    ColorMask 0
    Pass {}
  }
}