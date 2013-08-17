import QtQuick 2.0

Item {
  clip: true
  //プロパティの追加
  property alias gradient: rect.gradient
  property alias rotation: rect.rotation
  //グラデーションをかけて回転する四角
  Rectangle {
    id: rect
    anchors.centerIn: parent
    width: calcWidth()
    height: calcHeight()
    antialiasing: true
    //デフォルトグラデーション
    gradient: Gradient {
      GradientStop { position: 0.0; color: "Violet" }
      GradientStop { position: 1.0; color: "Navy" }
    }
    //ラジアンを予め計算しておく
    property real rad: calcRad()
    //角度は0～90になるように計算しておく
    function calcRad(){
      var r = rotation
      while(r > 90){
        r -= 90
      }
      while(r < 0){
        r += 90
      }
      return r * Math.PI / 180
    }
    //0～90度を基準に90度ごとにwidthとheightにかけるcosとsinが入れ替わる
    //そのためマイナス側の-90～0は、入れ替わりになるように90度ずらして判定する
    function calcWidth(){
      var len = 0
      var r = rotation
      if(r < 0){
        r = -(rotation - 90)
      }
      if((0 <= r && r <= 90) || (180 < r && r <= 270)){
        len = parent.width * Math.cos(rad) + parent.height * Math.sin(rad)
      }else if((90 < r && r <= 180) || (270 < r && r <= 360)){
        len = parent.width * Math.sin(rad) + parent.height * Math.cos(rad)
      }else{
        len = parent.width * Math.cos(rad) + parent.height * Math.sin(rad)
      }
      return len
    }
    function calcHeight(){
      var len = 0
      var r = rotation
      if(r < 0){
        r = -(rotation - 90)
      }
      if((0 <= r && r <= 90) || (180 < r && r <= 270)){
        len = parent.width * Math.sin(rad) + parent.height * Math.cos(rad)
      }else if((90 < r && r <= 180) || (270 < r && r <= 360)){
        len = parent.width * Math.cos(rad) + parent.height * Math.sin(rad)
      }else{
        len = parent.width * Math.sin(rad) + parent.height * Math.cos(rad)
      }
      return len
    }
  }
}
