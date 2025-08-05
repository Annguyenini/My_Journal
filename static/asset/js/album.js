console.log("album.js loaded!");
import { createButton } from "./gallerry.js";
let imageCard;
let globalData;
document.addEventListener("DOMContentLoaded", () => {
  const albumId = document.body.dataset.albumId;
  console.log("Loaded album:", albumId);
  displayWholeAlbum(albumId);
});
async function displayWholeAlbum(city){
    
    const res = await fetch(`/api/images/${city}`);
    const data = await res.json();
    globalData = data;
    const ablum = document.getElementById("grid");
    ablum.className="album";

    data.forEach(imgs => {
        imageCard = document.createElement("img");
        let imgsrc = imgs.src
        imageCard.src =imgsrc;
        imageCard.id = imgs.filename;
        imageCard.alt = city+" Image";
        ablum.appendChild(imageCard);
    }); 
    
}
let targetId;
document.addEventListener("click",(event)=>{
    targetId = event.target.id;
    
    // console.log(globalData);
    const imgObject = globalData.find(city => city.filename === targetId);
    if (imgObject){
        document.querySelector(".edit-card").innerHTML = "";
        
        renderEditCard(imgObject);
    }
});


function renderEditCard(imgObject){
    const editCard = document.querySelector(".edit-card");
    console.log("editCard exists?", document.querySelector(".editCard"));

    console.log("editCard:", editCard);
    console.log("Type of editCard:", typeof editCard);
    const metaData = document.createElement("div");
    metaData.className="metadata";
    metaData.textContent="asDSDSDSDSDSDSDSDSDSDSDSDSDSDSDdsdsds";
    
    const imageContainer = document.createElement("div");
    imageContainer.className="image-container";

    const image = document.createElement("img");
    image.src=imgObject.src;

    const exitBtn = createButton("X",'exitbtn',()=>{
        document.querySelector(".edit-card").innerHTML = "";
    });
    imageContainer.appendChild(image);
    editCard.appendChild(metaData);
    editCard.appendChild(imageContainer);
    editCard.appendChild(exitBtn);

}
