
import numpy as np
import operator
#test 1, states = 2, observational types = 2
transition= [[0,1],[1,0]]
priors= [0.5, 0.5]
emission = [[0,1],[0,1]]
observations = [[1,2]]

class HMM:

    def __init__(self):
        self.priors=None
        self.transition=None
        self.emission=None

    def viterbi (self,observations):
        """Return the best path, given an HMM model and a sequence of observations"""
        # A - initialise stuff
        nSamples = len(observations)
        nStates = self.transition.shape[0] # number of states
        c = np.zeros(nSamples) #scale factors (necessary to prevent underflow)
        viterbi = np.zeros((nStates,nSamples)) # initialise viterbi table
        psi = np.zeros((nStates,nSamples)) # initialise the best path table
        best_path = np.zeros(nSamples); # this will be your output

        # B- appoint initial values for viterbi and best path (bp) tables - Eq (32a-32b)
        viterbi[:,0] = self.priors.T * self.emission[:,observations[0]]
        c[0] = 1.0/np.sum(viterbi[:,0])
        viterbi[:,0] = c[0] * viterbi[:,0] # apply the scaling factor
        psi[0] = 0;
        print viterbi[:,0]

        # C- Do the iterations for viterbi and psi for time>0 until T
        for t in range(1,nSamples): # loop through time
            for s in range (0,nStates): # loop through the states @(t-1)
                trans_p = viterbi[:,t-1] * self.transition[:,s]
                psi[s,t], viterbi[s,t] = max(enumerate(trans_p), key=operator.itemgetter(1))
                viterbi[s,t] = viterbi[s,t]*self.emission[s,observations[t]]

            
            c[t] = 1.0/np.sum(viterbi[:,t]) # scaling factor
            viterbi[:,t] = c[t] * viterbi[:,t]
            print(viterbi[:,t])

        # D - Back-tracking
        best_path[nSamples-1] =  viterbi[:,nSamples-1].argmax() # last state
        for t in range(nSamples-1,0,-1): # states of (last-1)th to 0th time step
            best_path[t-1] = psi[best_path[t],t]

        return best_path

hmm = HMM()
hmm.priors = np.array([0.5, 0.5]) # pi = prior probs
hmm.transition = np.array([[0.75, 0.25], # A = transition probs. / 2 states
                           [0.32, 0.68]])
hmm.emission = np.array([[0.8, 0.1, 0.1], # B = emission (observation) probs. / 3 obs modes
                         [0.1, 0.2, 0.7]])

#print("Viterbi with transistion:"+str(transition)+" priors "+str(priors)+" observations " +str(observations)+" and emission:"+str(emission))
print hmm.viterbi([0,1,2,0,1,2])





