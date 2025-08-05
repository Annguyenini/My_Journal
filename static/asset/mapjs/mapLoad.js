import { callGallery } from '../js/gallerry.js';
var map;
const citylayer ={};
export function zoomTo(lat, lng) {
   map.setView([lat, lng], 7);
}
export function changeCallCityColor(cityname){
        citylayer[cityname].setStyle({
          fillColor:"red",
          fillOpacity: 0,
        });
      }
      

       document.addEventListener("DOMContentLoaded", () => {
        loadBaseMap();
        loadLayer();
      });
function loadBaseMap(){
    fetch("../static/logs/log.json")
            .then((response) => response.json())
            .then((data) => {
                console.log(data);

                map = L.map("map").setView([16.30,107.05], 5);
                const Vietnambound = L.latLngBounds(
                [8.18, 102.14],
                [23.39, 109.46]
                );
                L.tileLayer("https://mt1.google.com/vt/lyrs=r&x={x}&y={y}&z={z}", {
                // L.tileLayer("../static/asset/tiles/{z}/{x}/{y}.png",{
                // attribution:
                //   '&copy; <a href="https://www.openstreetmap.org/">OpenStreetMap</a> contributors',
                minZoom: 4,
                maxZoom:18,
                opacity: 0,
                bounds: Vietnambound,
                attribution: 'Offine Map',
                tileSize: 256,
                noWrap: true,
                errorTileUrl:"./static/asset/tiles/blank.png",
                }).addTo(map);

                data.cities.forEach(city => {
                const cityName = city.name;
                console.log(cityName);
                const lat = parseFloat(city.properties.lat);
                const lng = parseFloat(city.properties.lng);
                console.log(lat);
                console.log(lng);
                L.marker([lat, lng]).addTo(map).bindPopup("sd");
                });
            
                map.setMaxBound(Vietnambound);
                map.on("drag", () =>
                map.panInsideBounds(vietnamBounds, { animate: false })
                );
            })
            .catch((err) => console.error("Falide", err));
        }


   async function loadLayer(){
    const res = await fetch("../static/asset/mapjs/vn.json");
    const cities = await res.json();
    L.geoJSON(cities, {
              style: {
                color: "#000",
                fillColor: "#3498db",
                fillOpacity: 0.5,
                weight: 1,
              },
              onEachFeature: function (feature, layer) {
                let name = feature.properties.name;
                citylayer[name] = layer;
                layer.on({
                  // mouseover: function (e) {
                  //   layer.setStyle({ fillColor: "#e74c3c",color: " #FF0000" });
                  // },
                  // mouseout: function (e) {
                  //   layer.setStyle({ fillColor: "#3498db", color: "#000" });
                  // },
                  click: function (e) {
                    // alert(feature.properties.name);\
                    console.log(feature.properties.name);
                    callGallery(feature.properties.name);
                    
                  },
                });
              },
            }).addTo(map);
        }
