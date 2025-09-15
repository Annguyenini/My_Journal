import {Map} from "../mapjs/mapLoad.js";
export class ImageSlider   {
    constructor(imageCache){
        this.imageCache = imageCache;
        this.setUpDiv();
        this.mapObject = new Map();
        this.curentImageId;
    };
    getSlider(){
        return this.sliderCard;
    }
    createButton(buttonName, className){
        const button = document.createElement('button');
        button.className = className;
        button.textContent = buttonName;
        button.type = "button";
        return button;
    }
    setUpDiv(){
        console.log("called");
        this.sliderCard = document.getElementById("slider");
        this.sliderCard.className="slider";
        this.cardHeader = document.createElement("div");
        this.cardHeader.className="slider-header";
        this.cardBottom = document.createElement('div');
        this.cardBottom.className ="slider-bot";
        this.className="slider-bottom";
        this.setupCardInfo();
        this.setupImageCard();
        this.setupImagesButtons();
        this.setUpButtons();
        this.cardHeader.appendChild(this.cardInfo);
        this.cardHeader.appendChild(this.imageFrame);
        this.cardBottom.appendChild(this.buttonFrame);
        this.sliderCard.appendChild(this.cardHeader);
        this.sliderCard.appendChild(this.imageFrame);

        this.sliderCard.appendChild(this.cardBottom);

    }
    
    setupCardInfo(){
        this.cardInfo = document.createElement('div');
        this.cardInfo.className="slider-image-info-box";
       

        this.cardInfoCity = document.createElement('h2');
        this.cardInfoCity.className="silder-card-title";

        this.cardInfoCoor = document.createElement('h5');
        this.cardInfoCoor.className="silder-info-coors";

        this.cardInfoTime = document.createElement('h5');
        this.cardInfoTime.className="slider-info-time";

        this.cardInfo.appendChild(this.cardInfoCity);
        this.cardInfo.appendChild(this.cardInfoCoor);
        this.cardInfo.appendChild(this.cardInfoTime);
         
    }
    setUpButtons(){
        this.exitBtn = this.createButton("X","exit-btn");
        this.exitBtn.type="button";
        this.exitBtn.addEventListener('click',()=>{
            this.closeSlider();
            
        })
        this.cardInfo.appendChild(this.exitBtn);
    }
    setupImageCard(){
        this.imageFrame = document.createElement('div');
        this.imageFrame.className= "slider-image-frame";
        this.imageCard = document.createElement('img');
        this.imageCard.className = "slider-image";
        this.imageFrame.appendChild(this.imageCard);
    }
    setupImagesButtons(){
        this.nextBtn = this.createButton("Next","images-function-button");
        this.nextBtn.addEventListener('click',() => {
            this.callNextbutton();
        });
        this.prevBtn = this.createButton("Previous","images-function-previous");
        this.prevBtn.addEventListener('click',() => {
            this.callPrevbutton();
        });
        this.downloadBtn = this.createButton("Download","images-function-download");
        this.downloadBtn.addEventListener('click',()=>{
            this.callDownloadbutton(this.imageCache[this.curentImageIndex].path);
        })
        
        this.deleteBtn = this.createButton("Delete","images-function-delete");
        this.deleteBtn.addEventListener('click',()=>{
            if(window.confirm ("Are you sure?")){
                this.callDeletebutton(this.imageCache[this.curentImageIndex].path);
            }

        })
        this.buttonFrame = document.createElement('div');
        this.buttonFrame.className="slider-buttons-frame";
        this.buttonFrame.appendChild(this.nextBtn);
        this.buttonFrame.appendChild(this.prevBtn);
        this.buttonFrame.appendChild(this.downloadBtn);
        this.buttonFrame.appendChild(this.deleteBtn);
    }
    callImageSliderFromMap(imagePath){
        this.curentImageIndex = this.imageCache.findIndex((image) => imagePath ===image.path);
        this.callImageSlider()
        
    }
    callImageSlider(){
        this.imageCard.src=`/static/logs/gallery/${this.imageCache[this.curentImageIndex].path}`;
        this.cardInfoCity.textContent = this.imageCache[this.curentImageIndex].tripName;
        const lat = this.imageCache[this.curentImageIndex].lat;
        const lng = this.imageCache[this.curentImageIndex].lng;
        const time = this.imageCache[this.curentImageIndex].time;
        this.cardInfoCoor.textContent = `lat: ${lat} | lng: ${lng}`;
        this.cardInfoTime.textContent = `Time: ${time}`;
    }
    callNextbutton(){
        let index = this.curentImageIndex +1;
        if(index >= this.imageCache.length){
            index = 0;
        }
        this.curentImageIndex =index ;
        this.curentImageId = this.imageCache[this.curentImageIndex].time;
        this.callImageSlider();
        let coor = [this.imageCache[this.curentImageIndex].lat,this.imageCache[this.curentImageIndex].lng];
        console.log(coor);
        this.mapObject.zoomToCoor(coor,17);
    
        
    }
    callPrevbutton(){
        
        let index = this.curentImageIndex -1;
        if(index <= 0){
            index = this.imageCache.length-1;
        }
        this.curentImageIndex =index ;
        this.curentImageId = this.imageCache[this.curentImageIndex].time;
        this.callImageSlider();
        let coor = [this.imageCache[this.curentImageIndex].lat,this.imageCache[this.curentImageIndex].lng];
        console.log(coor);
        this.mapObject.zoomToCoor(coor,17);
        
    }
    async callDownloadbutton(imageId){
        const res = await fetch(`/static/logs/gallery/${imageId}`);
        const imageBlog = await res.blob();
        const imageURL = URL.createObjectURL(imageBlog);
        const link = document.createElement('a');
        link.href = imageURL;
        link.download= imageId;
        link.click();
        link.remove();
        URL.revokeObjectURL(imageURL);
    }
    async callDeletebutton(imageId){
        const el = this.imageCache.find((element)=> element.path === imageId);
        const coor = el ? [el.lat,el.lng] :null;
        console.log(coor);
        this.mapObject.hideAMarker(coor);
        const res = await fetch('/api/delete',{
            method: "POST",
            headers:{
                "Content-Type":"application/json"
            },
            body: JSON.stringify({imageid: imageId})
        });
        const message = await res.json();
        console.log("delete status: ", message)
        this.callNextbutton();
        
    }
    closeSlider(){
        const main = document.getElementById('map');
        const slider = document.getElementById('slider');
        main.classList.toggle('shifted');
        slider.classList.toggle('active');
        this.curentImageId =null;
        let coor = [this.imageCache[this.curentImageIndex].lat,this.imageCache[this.curentImageIndex].lng];
        console.log(coor);
        this.mapObject.zoomToCoorN(coor);
    }
    transform(imageid){
    const main = document.getElementById('map');
    const slider = document.getElementById('slider');
    if(this.curentImageId!=null){
      console.log(imageid);
      console.log(this.curentImageId);
      if(this.curentImageId===imageid){
        console.log("called");
        main.classList.toggle('shifted');
        slider.classList.toggle('active');
      }
      
    }
    else{
      console.log("else");
      main.classList.toggle('shifted');
      slider.classList.toggle('active');
    }
    this.curentImageId = imageid;
    console.log(this.curentImageId)
  }
};
