
//HTTPにアクセスし受信内容を確認します
function requestHttp( data , method , url , async, on_func ){
    requestHttpEx(data, method, url, async, on_func, null);
}

function requestHttpEx( data , method , url , async, on_func, on_func_param )
{
    //XMLHttpRequestオブジェクト生成
    var httpoj = new XMLHttpRequest();

    //open メソッド
    httpoj.open( method , url , async );

    //受信時に起動するイベント
    httpoj.onreadystatechange = function(){
        //readyState値は4で受信完了
        if (httpoj.readyState==4){
            //コールバック
            if(on_func !== null && on_func_param !== null){
                on_func(httpoj, on_func_param);
            }else if(on_func !== null){
                on_func(httpoj);
            }else{
                on_loaded(httpoj);
            }
        }
    }
    //send メソッド
    httpoj.send( data );
}

//コールバック関数 ( 受信時に実行されます )
function on_loaded(oj)
{
    //レスポンスを取得
    var res  = oj.responseText;

    //ダイアログで表示
    console.log(res);
}
