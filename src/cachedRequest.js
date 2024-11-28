const MaximumCacheSize = 20
export class CachedRequest {
    constructor(userName, request) {
        this.userName = userName;
        this.request = request;
    }
}

export class CachedRequestArray{
    constructor() {
        this.requests = []
        this.AddCachedRequest(new CachedRequest("Default", ""))
    }
    
    GetRequest(userName){
        for (var i = 0; i < this.requests.length; i++){
            if (this.requests[i].userName === userName){
                return this.requests[i]
            }
        }
        return null;
    }
    
    SetRequest(userName, request){
        for (var i = 0; i < this.requests.length; i++){
            if (this.requests[i].userName === userName){
                this.requests[i].request = request
                return this.requests[i]
            }
        }
        
        //If not found create new
        this.AddCachedRequest(new CachedRequest(userName, request))
        return this.requests[0]
    }
    AddCachedRequest(cachedRequest){
        if (this.requests.length >= MaximumCacheSize){
            this.requests.pop()
        }
        this.requests.splice(0, 0, cachedRequest)
    }
}