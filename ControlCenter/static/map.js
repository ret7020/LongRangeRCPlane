// initMap();

// async function initMap() {
//     await ymaps3.ready;
//     const { YMap, YMapDefaultSchemeLayer } = ymaps3;
//     const map = new YMap(
//         document.getElementById('map'),
//         {
//             location: {
//                 center: [37.588144, 55.733842],
//                 zoom: 10
//             }
//         }
//     );

//     map.addChild(new YMapDefaultSchemeLayer());
// }


var view = new ol.View({
    center: ol.proj.fromLonLat([37.623210, 55.752563]),
    zoom: 16
});

var tileLayer = new ol.layer.Tile({
    source: new ol.source.XYZ({
        url: 'http://{a-c}.tile.openstreetmap.org/{z}/{x}/{y}.png'
    })
});

const features = [new ol.Feature({
    geometry: new ol.geom.Point(ol.proj.fromLonLat([37.623210, 55.752563]))
})];

const vectorSource = new ol.source.Vector({
    features
});

const vectorLayer = new ol.layer.Vector({
    source: vectorSource,
    style: new ol.style.Style({
        image: new ol.style.Circle({
            radius: 10,
            fill: new ol.style.Fill({ color: 'red' })
        })
    })
});


var map = new ol.Map({
    layers: [
        tileLayer,
        vectorLayer
    ],
    target: 'map',
    view: view
});

// Simulate
// Dev only

setInterval(function () {
    
}, 1000);