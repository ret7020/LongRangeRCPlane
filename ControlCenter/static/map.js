var view = new ol.View({
    center: ol.proj.fromLonLat([37.623210, 55.752563]),
    zoom: 12
});

var tileLayer = new ol.layer.Tile({
    source: new ol.source.XYZ({
        url: 'http://{a-c}.tile.openstreetmap.org/{z}/{x}/{y}.png'
    })
});

var planePoint = new ol.Feature({
    geometry: new ol.geom.Point(ol.proj.fromLonLat([37.623210, 55.752563]))
});

const dynamiCobjects = [planePoint];

const vectorSource = new ol.source.Vector({
    features: dynamiCobjects
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


var trajectory = new ol.Feature({
    geometry: new ol.geom.LineString([])
});

var trajectoryLayer = new ol.layer.Vector({
    source: new ol.source.Vector({
        features: [trajectory]
    }),
    style: new ol.style.Style({
        stroke: new ol.style.Stroke({
            color: 'red',
            width: 3
        })
    })
});









// Sat view
// new ol.layer.Tile({
//     source: new ol.source.XYZ({
//       url: 'https://server.arcgisonline.com/ArcGIS/rest/services/World_Imagery/MapServer/tile/{z}/{y}/{x}',
//       maxZoom: 19
//     })
//   })

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
var planeLon = 37.623210;
var planeLat = 55.752563;

setInterval(function () {
    planeLon += Math.random() * (0.002 - (-0.0001)) + (-0.0001);
    planeLat += Math.random() * (0.002 - (-0.0001)) + (-0.0001);
    planePoint.getGeometry().setCoordinates(ol.proj.fromLonLat([planeLon, planeLat]))
    trajectory.getGeometry().appendCoordinate(ol.proj.fromLonLat([planeLon, planeLat]));
    // planeLon -= 0.0005;
}, 500);







map.addLayer(trajectoryLayer);