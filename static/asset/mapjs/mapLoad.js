



import { callGallery } from '../js/gallerry.js';
import {ImageSlider} from '../js/imagesilder.js';
export class Map{
  static map;
  constructor(){
    
    this.imageCache =[];
    this.coors =[];
    
    this.tripId = document.body.dataset.tripId;
    this.startIcon = L.icon({
    iconUrl: '/static/css/images/startpin.png',
    shadowUrl: 'path/to/your/shadow.png',
    iconSize: [ 50, 50],
    
  });
  }
  async fetchGps(){
    console.log(this.tripId);
    const res = await fetch(`/api/getdatas/${this.tripId}`);
    
    this.tripDatas = await res.json();
  }
 

  getCustomIcon(path){
    let icon= L.divIcon({
     html: `<div class="marker-border">
             <img src=${path} />
           </div>`,
    iconSize: [ 10, 10],
    
    });
    return icon;
  }
  zoomToCoor(coor){
    let offsetLng = coor[1] - 0.0040; // tweak this value for desired shift

    Map.map.flyTo([coor[0],offsetLng],17);
  }
   zoomToCoorN(coor){
    

    Map.map.flyTo(coor,17);
  }
  
  loadBaseMap(){
    Map.map = L.map("map").setView([16.30,107.05], 5);
    // L.tileLayer("https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png", {
    L.tileLayer("https://mt1.google.com/vt/lyrs=r&x={x}&y={y}&z={z}", {
    // L.tileLayer("../static/asset/tiles/{z}/{x}/{y}.png",{
    // attribution:
    //   '&copy; <a href="https://www.openstreetmap.org/">OpenStreetMap</a> contributors',
    minZoom: 4,
    maxZoom:22,
    // opacity: 0,
    // bounds: Vietnambound,
    attribution: 'Offine Map',
    tileSize: 256,
    noWrap: true,
    errorTileUrl:"./static/asset/tiles/blank.png",
    }).addTo(Map.map);
  };
  async processData(){
    await this.fetchGps();
    this.tripDatas.forEach(data => {
      
      let coor = [data.lat,data.lng];
      this.coors.push(coor);
      if(data.path != null){
        const icon = this.getCustomIcon(`/static/logs/gallery/${data.path}`);
        console.log(`/static/logs/gallery/${data.path}`);
      L.marker(coor,{icon: icon}).on('click',()=>{
        this.zoomToCoor(coor);
        this.callSlider(data.path);
        this.sliderObject.transform(data.time);
        }).addTo(Map.map);
        this.imageCache.push(data);
      }
      
      
    });
    console.log(this.imageCache);
  }
  async loadTripMap()
  {
    await this.processData();
    let startcoor = this.coors[0];
    console.log(startcoor);
    Map.map.setView(startcoor,15);
    L.marker(startcoor,{icon: this.startIcon}).on('click',()=>{
      console.log("sasdasdad");
    }).addTo(Map.map);

    L.polyline(this.coors,{color:'red'}).addTo(Map.map);
    this.sliderObject = new ImageSlider(this.imageCache);
    this.silderCard = this.sliderObject.getSlider();
  }
  callSlider(imagepath){
   
    this.sliderObject.callImageSliderFromMap(imagepath);
  }
  hideAMarker(coor){
    Map.map.eachLayer((layer)=>{
      if (layer instanceof L.Marker){
        const latlng = layer.getLatLng();
        if(latlng.lat ===coor[0]&& latlng.lng === coor[1]){
        Map.map.removeLayer(layer);
      }
      }
    })
  }
};

let m = new Map();
m. loadBaseMap();
m.loadTripMap();
// var map;
// const citylayer ={};
// export function zoomTo(lat, lng) {
//    map.setView([lat, lng], 7);
// }
// export function changeCallCityColor(cityname){
//         citylayer[cityname].setStyle({
//           fillColor:"red",
//           fillOpacity: 0,
//         });
//       }
      

//        document.addEventListener("DOMContentLoaded", () => {
//         loadBaseMap();
//         // loadLayer();
//       });
// function loadBaseMap(){
//     fetch("/api/get")
//             .then((response) => response.json())
//             .then((data) => {
//                 console.log(data);

//                 map = L.map("map").setView([16.30,107.05], 5);
//                 L.tileLayer("https://mt1.google.com/vt/lyrs=r&x={x}&y={y}&z={z}", {
//                 // L.tileLayer("../static/asset/tiles/{z}/{x}/{y}.png",{
//                 // attribution:
//                 //   '&copy; <a href="https://www.openstreetmap.org/">OpenStreetMap</a> contributors',
//                 minZoom: 4,
//                 maxZoom:18,
//                 // opacity: 0,
//                 // bounds: Vietnambound,
//                 attribution: 'Offine Map',
//                 tileSize: 256,
//                 noWrap: true,
//                 errorTileUrl:"./static/asset/tiles/blank.png",
//                 }).addTo(map);

                
//                 var polyline = L.polyline(latlngs,{color:'red'}).addTo(map);
//                 data.cities.forEach(city => {
//                 const cityName = city.name;
//                 console.log(cityName);
//                 // const lat = parseFloat(city.properties.lat);
//                 // const lng = parseFloat(city.properties.lng);
//                 // console.log(lat);
//                 // console.log(lng);
//                 // L.marker([lat, lng]).addTo(map).bindPopup("sd");
//                 });
            
//                 map.setMaxBound(Vietnambound);
//                 map.fitBounds(polyline.getBounds());

//                 map.on("drag", () =>
//                 map.panInsideBounds(vietnamBounds, { animate: false })
//                 );
//             })
//             .catch((err) => console.error("Falide", err));
//         }


//    async function loadLayer(){
//     const res = await fetch("../static/asset/mapjs/vn.json");
//     const cities = await res.json();
//     L.geoJSON(cities, {
//               style: {
//                 color: "#000",
//                 fillColor: "#3498db",
//                 fillOpacity: 0.5,
//                 weight: 1,
//               },
//               onEachFeature: function (feature, layer) {
//                 let name = feature.properties.name;
//                 citylayer[name] = layer;
//                 layer.on({
//                   // mouseover: function (e) {
//                   //   layer.setStyle({ fillColor: "#e74c3c",color: " #FF0000" });
//                   // },
//                   // mouseout: function (e) {
//                   //   layer.setStyle({ fillColor: "#3498db", color: "#000" });
//                   // },
//                   click: function (e) {
//                     // alert(feature.properties.name);\
//                     console.log(feature.properties.name);
//                     callGallery(feature.properties.name);
                    
//                   },
//                 });
//               },
//             }).addTo(map);
//         }
