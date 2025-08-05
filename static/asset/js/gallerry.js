import { zoomTo,changeCallCityColor } from "../mapjs/mapLoad.js";
export async function callGallery(selectedCity) {
  removeOldCard();

  const city = await getCityData(selectedCity);
  if (!city) return renderEmptyCard(selectedCity);
  let imagesArray =[];
   imagesArray = await fetchImages(city.name);
  if (!imagesArray || imagesArray.length ===undefined|| imagesArray.length===0) {
    console.log("images corrupt or empty");
    renderEmptyCard(selectedCity);
    return;
  }
  console.log(imagesArray);
  renderGalleryCard(city, imagesArray);
}

// Clears any previous gallery card
function removeOldCard() {
  const oldCard = document.querySelector(".card");
  if (oldCard) oldCard.remove();
}

// Loads city info from log.json
async function getCityData(cityName) {
  const response = await fetch("../static/logs/log.json");
  const data = await response.json();
  const city = data.cities.find(city => city.name === cityName);
  console.log(city);
  if (city) {
    zoomTo(parseFloat(city.properties.lat), parseFloat(city.properties.lng));
    changeCallCityColor(city.name);
     // parse floats just in case
    return city;
  }
}

// Fetches image array from API
async function fetchImages(cityName) {
  const res = await fetch(`/api/images/${cityName}`);
  return await res.json();
}

// Render empty placeholder card
function renderEmptyCard(cityName) {
  const gallery = document.getElementById("gallery");
  const card = document.createElement("div");
  card.className = "card";
  card.id = cityName;
  card.textContent = `${cityName} Empty`;
  card.style.width='auto';
  card.style.height='auto';
  card.style.background ='white';
  gallery.appendChild(card);
}

// Build & show full gallery card
function renderGalleryCard(city, imagesArray) {
  const gallery = document.getElementById("gallery");
  const card = document.createElement("div");
  card.className = "card";
  card.id = city.name;
  const cardOverlay = document.createElement("div");
  cardOverlay.className ="polaroid-overlay";


  let currentIndex = 0;
  let currentImageSrc;

  // Elements
  const img = createImageElement(imagesArray[currentIndex]);
  const caption = createCaption(imagesArray[currentIndex].des);
  const locationOnCard = createLocation(imagesArray[currentIndex].loc);
  const timeStampOnCard = createTimeStamp(imagesArray[currentIndex].time);
  const exitBtn = createButton("X", "exit-btn", () => card.remove());
  const moreBtn = createButton("...", "more-btn",()=>{
  
  
  if(document.getElementById("delete-img-btn")){
    
    document.querySelector(".card #delete-img-btn").remove();
    document.querySelector(".card #download-img-btn").remove();

    return;
  };
  // const albumBtn = createButton("Album","album-btn", () =>{
  //     window.location.href=
  // });
  const delBtn = createButton("Delete Img", "delete-btn",() => {
    handleDelete(city.name,currentImageSrc);
    imagesArray.splice(currentIndex,1);
    
    fetchImages(city.name);
    if(currentIndex === imagesArray.length){
      currentIndex--;
    }
    if( imagesArray.length === 0){
      card.remove()
      renderEmptyCard(city.name);
    }
    
    updateImage();
    
  });
  const downloadBtn = createButton("Download","download-btn",() => {
    const imageSrc = imagesArray[currentIndex].src;
    const imageName =imagesArray[currentIndex].filename; 
    fetch(imageSrc)
    .then(res => res.blob())
    .then( blob =>{
      const url = URL.createObjectURL(blob);
      const link =document.createElement('a');
      link.href = imageSrc;
      link.download =imageName;
      document.body.appendChild(link);
      link.click();
      link.remove();
      URL.revokeObjectURL(url);
    });
  });
  downloadBtn.id="download-img-btn";
  delBtn.id = "delete-img-btn";
  card.appendChild(downloadBtn);
  card.appendChild(delBtn);
  });
  
  const prevBtn = createButton("<", "nav-btn prev", () => {
    if (currentIndex > 0) {
      currentIndex--;
      
      updateImage();
    }
  });
 gallery.addEventListener('scroll', () => {
  const buffer = 100; // How close to the bottom before triggering
  const isAtBottom = gallery.scrollTop + gallery.clientHeight >= gallery.scrollHeight - buffer;

  if (isAtBottom && currentIndex < imagesArray.length - 1) {
    currentIndex++;
    
    updateImage();
  }
  setTimeout(() => {
      isLoading = false;
    }, 3000);
});
  const nextBtn = createButton(">", "nav-btn next", () => {
    if (currentIndex < imagesArray.length - 1) {
      currentIndex++;
      updateImage();
    }
    
  });

  // Updates image + caption
  function updateImage() {
    
    const imageData = imagesArray[currentIndex];
    console.log(imageData.filename);  
    currentImageSrc = imageData.filename;
    img.src = imageData.src;
    img.id = imageData.filename;
    caption.textContent = imageData.des || "(No description)";
    locationOnCard.textContent = imageData.loc || "Unknown";
    timeStampOnCard.textContent =imageData.time ||"Unkown";
    prevBtn.disabled = (currentIndex === 0);
    nextBtn.disabled = (currentIndex === imagesArray.length - 1);
  }

  // Compose Card
  card.appendChild(img);
  card.appendChild(caption);
  card.appendChild(locationOnCard);
  card.appendChild(timeStampOnCard);
  card.appendChild(prevBtn);
  card.appendChild(nextBtn);
  card.appendChild(exitBtn);
  card.appendChild(moreBtn);
  card.appendChild(cardOverlay);
 
  gallery.appendChild(card);

  updateImage(); // Initial image
}

// Creates image element
function createImageElement(imageData) {
  const img = document.createElement("img");
  img.src = imageData.src;
  img.id = imageData.filename;
  img.alt = "Gallery Image";
  img.className = "gallery-img";
  return img;
}


// Creates caption element
function createCaption(text) {
  const caption = document.createElement("div");
  caption.className = "image-caption-overlay";
  caption.textContent = text || "(No description)";
  return caption;
}

function createLocation(text){
  const locationOnCard = document.createElement("div");
  locationOnCard.className="image-location-overlay";
  locationOnCard.textContent = text || "Unknow";
  return locationOnCard;
 }

function createTimeStamp(text){
  const timeStampOnCard = document.createElement("div");
  timeStampOnCard.className ="image-timestamp-overlay";
  timeStampOnCard.textContent = text || "Unknow";
  return timeStampOnCard;
}

// Generic button creator
export function createButton(text, className, onClick) {
  const btn = document.createElement("button");
  btn.textContent = text;
  btn.className = className;
  btn.type = "button";
  btn.addEventListener("click", onClick);
  return btn;
}

// Handle delete request
function handleDelete(cityname, filename) {
  console.log(cityname+filename);
  if (confirm("Are you sure you want to remove the image?")) {
    fetch("/api/imagedelete", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        city:cityname, 
        filename:filename })
      
    })
      .then(res => res.json())
      .then(data => {
        console.log("Server said:", data);
      });
  }
}

