class MainLayout {
    async fetchTrips() {
        const res = await fetch('/api/gettrips');
        this.tripslist = await res.json();
    }

    async fetchCurentTripId() {
        const res = await fetch('/api/getcurenttripid');
        let crentTripID = await res.json();
        this.curentTripId = Number(crentTripID);
        console.log(this.curentTripId,typeof this.curentTripId);
        this.curentTrip = this.tripslist.find(trip => trip.id === this.curentTripId);
        console.log(this.curentTrip);

    }

    constructor() {
        this.tripslist = [];
        this.curentTrip = null;
        this.curentTripId = null;
    }

    renderCurentTrip() {
        this.tripCard = document.getElementById('currentTrip');
        if (!this.tripslist || this.tripslist.length === 0) {
            console.log("empty");
            const card = document.createElement("div");
            card.className = "maincard";
            card.id = "emptycard";
            card.textContent = "EMPTYYYYYYYYYYYYY";
            card.style.width = 'auto';
            card.style.height = 'auto';
            card.style.background = 'white';
            this.tripCard.appendChild(card);
        } else {
            const curentTripCard = document.createElement('div');
            curentTripCard.className = "maincard";
            curentTripCard.id = "maintrip";
            curentTripCard.textContent = this.curentTrip.trip_name;
            const image = document.createElement('img'); // placeholder
            curentTripCard.appendChild(image);
            curentTripCard.style.cursor= "pointer";
            curentTripCard.addEventListener("click",()=>{
                window.location.href = `/trip/${this.curentTrip.id}`;

            })
            this.tripCard.appendChild(curentTripCard);
        }
    }

    renderPreviouTrips() {
        const previousTripList = document.getElementById("pastTrips");
        
        
        this.tripslist.forEach(trip => {
            const tripCard = document.createElement("div");
            tripCard.id = trip.tripId;
            tripCard.textContent = trip.trip_name;
            tripCard.style.cursor = "pointer";

            tripCard.addEventListener("click", () => {
                window.location.href = `/trip/${trip.id}`;
            });
            
            previousTripList.appendChild(tripCard);
        });
    }

    async renderTrip() {
        await this.fetchTrips();
        await this.fetchCurentTripId();
        this.renderCurentTrip();
        this.renderPreviouTrips();
        console.log("start");
    }
}

document.addEventListener("DOMContentLoaded", () => {
    const ml = new MainLayout();
    ml.renderTrip();
    console.log("triplist.js loaded!");

});
