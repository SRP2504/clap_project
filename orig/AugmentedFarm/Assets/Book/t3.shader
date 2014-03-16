Shader "t3" {
  SubShader {
    Tags {"Queue" = "Overlay   +10" } // earlier = hides stuff later in queue
    Lighting Off
    ZTest LEqual
    ZWrite On
    ColorMask 0
    Pass {}
  }
}