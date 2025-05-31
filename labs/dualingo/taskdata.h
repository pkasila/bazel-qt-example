#ifndef TASKDATA_H
#define TASKDATA_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QRandomGenerator> 
#include <algorithm>    
#include <QMap>
#include <QDebug>

enum class ExerciseType {
    None,
    Translation, 
    Grammar
};

struct TaskData {
    ExerciseType type;
    QString prompt;
    QString answer; 
    QStringList options; 
    int correctAnswerIndex; 
    QString hint;
    QString originalDifficulty; 
};

inline QList<TaskData> getAllHardcodedTasks() {
    QList<TaskData> allTasks;

    // ==========================================
    // EASY - Translation/Vocabulary (ExerciseType::Translation)
    // ==========================================
    allTasks.append({ExerciseType::Translation, "What is a common pet that barks?", "dog", {}, -1, "It's known as 'man's best friend'.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What color is the sky on a sunny day?", "blue", {}, -1, "Think of a primary color.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Complete: Apples ______ on trees.", "grow", {}, -1, "What happens to fruit before we pick it?", "Easy"});
    allTasks.append({ExerciseType::Translation, "What is the opposite of 'hot'?", "cold", {}, -1, "You feel this in winter.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What is a synonym for 'big'?", "large", {}, -1, "Another word for great size.", "Easy"});
    allTasks.append({ExerciseType::Translation, "How many days are in a week?", "seven", {}, -1, "Starts with Sunday or Monday.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What do bees make?", "honey", {}, -1, "A sweet, sticky food.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Complete: We use our ______ to see.", "eyes", {}, -1, "You have two of them on your face.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What is the opposite of 'happy'?", "sad", {}, -1, "How you feel when something bad happens.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What sound does a cat make?", "meow", {}, -1, "A common feline vocalization.", "Easy"});
    allTasks.append({ExerciseType::Translation, "A place with many books is called a ______.", "library", {}, -1, "You can borrow books here.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What is a synonym for 'small'?", "little", {}, -1, "Not big.", "Easy"});
    allTasks.append({ExerciseType::Translation, "The sun ______ during the day.", "shines", {}, -1, "It gives light and warmth.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Opposite of 'fast'?", "slow", {}, -1, "Like a turtle.", "Easy"});
    allTasks.append({ExerciseType::Translation, "A fruit that is long and yellow.", "banana", {}, -1, "Monkeys love them.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What do you wear on your feet?", "shoes", {}, -1, "Protects your feet when walking.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Complete: Birds can ______ in the sky.", "fly", {}, -1, "They use their wings.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Opposite of 'day'?", "night", {}, -1, "When the moon is visible.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'quick'?", "fast", {}, -1, "Moving at high speed.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What animal says 'oink'?", "pig", {}, -1, "Often found on a farm.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What do you drink when you are thirsty?", "water", {}, -1, "Clear liquid essential for life.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Complete: A ______ helps sick people.", "doctor", {}, -1, "Works in a hospital or clinic.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Opposite of 'open'?", "closed", {}, -1, "Like a door that is not open.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'glad'?", "happy", {}, -1, "Feeling pleasure or contentment.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What do you use to write?", "pen", {}, -1, "Or a pencil.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Complete: Fish ______ in water.", "swim", {}, -1, "They use fins.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What color are strawberries?", "red", {}, -1, "A common color for fruits.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Opposite of 'young'?", "old", {}, -1, "Having lived for a long time.", "Easy"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'begin'?", "start", {}, -1, "To commence an action.", "Easy"});
    allTasks.append({ExerciseType::Translation, "What is the main meal of the evening?", "dinner", {}, -1, "Usually eaten after work or school.", "Easy"});


    // ==========================================
    // EASY - Grammar (ExerciseType::Grammar)
    // ==========================================
    allTasks.append({ExerciseType::Grammar, "She ______ an apple.", "", {"eat", "eats", "eating"}, 1, "Present simple, third person singular.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "They ______ playing in the park.", "", {"is", "are", "am"}, 1, "Plural subject, present continuous.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "This is ______ cat.", "", {"a", "an", "the"}, 0, "Use 'a' before a consonant sound.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "I ______ happy.", "", {"is", "are", "am"}, 2, "First person singular, verb 'to be'.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "He ______ a blue car.", "", {"has", "have", "haves"}, 0, "Third person singular, verb 'to have'.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "The book is ______ the table.", "", {"on", "in", "under"}, 0, "Indicates position on a surface.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "We ______ to school every day.", "", {"go", "goes", "going"}, 0, "Present simple, plural subject.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "______ are you from?", "", {"What", "Where", "Who"}, 1, "Asking about location.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "My mom is ______ teacher.", "", {"a", "an", "the"}, 0, "Article for a profession.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "Can you help ______?", "", {"I", "me", "my"}, 1, "Object pronoun.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "There ______ many birds.", "", {"is", "are"}, 1, "For plural countable nouns.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "This is ______ orange.", "", {"a", "an"}, 1, "Use 'an' before a vowel sound.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "The dogs ______ running.", "", {"is", "are"}, 1, "Plural subject.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "What ______ your name?", "", {"is", "are"}, 0, "Singular subject 'name'.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "I like ______ apples.", "", {"this", "these", "that"}, 1, "Plural demonstrative.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "He ______ (read) a book now.", "", {"read", "reads", "is reading"}, 2, "Present continuous for an action happening now.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "Are ______ (they) students?", "", {"they", "them", "their"}, 0, "Subject pronoun.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "The flowers are ______ (beauty).", "", {"beauty", "beautiful", "beautifully"}, 1, "Adjective form.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "Look! It ______ (rain).", "", {"rain", "rains", "is raining"}, 2, "Action happening at the moment of speaking.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "My brother is taller ______ me.", "", {"then", "than", "that"}, 1, "Used for comparisons.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "She ______ (not like) coffee.", "", {"don't like", "doesn't like", "not likes"}, 1, "Negative present simple, third person.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "What time ______ it?", "", {"is", "are", "do"}, 0, "Asking about time.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "These are ______ (my) keys.", "", {"my", "me", "I"}, 0, "Possessive adjective.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "We went to the zoo ______ Saturday.", "", {"on", "in", "at"}, 0, "Preposition for days of the week.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "Can I have ______ water, please?", "", {"some", "any", "a"}, 0, "For requests, uncountable nouns.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "He is ______ honest man.", "", {"a", "an", "the"}, 1, "'h' is silent.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "Children ______ to play.", "", {"like", "likes", "is liking"}, 0, "Plural subject, present simple.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "This is my friend. ______ name is Tom.", "", {"His", "Her", "Its"}, 0, "Possessive pronoun for a male.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "I have two ______ (sister).", "", {"sister", "sisters", "sister's"}, 1, "Plural noun.", "Easy"});
    allTasks.append({ExerciseType::Grammar, "The cat is sleeping ______ the sofa.", "", {"on", "under", "in"}, 0, "Position on a surface.", "Easy"});


    // ==========================================
    // MEDIUM - Translation/Vocabulary (ExerciseType::Translation)
    // ==========================================
    allTasks.append({ExerciseType::Translation, "Synonym for 'intelligent'?", "smart", {}, -1, "Clever or bright.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Antonym for 'polite'?", "rude", {}, -1, "Not showing good manners.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Complete: To achieve your goals, you must ______.", "persevere", {}, -1, "Keep trying despite difficulties.", "Medium"});
    allTasks.append({ExerciseType::Translation, "What does 'ubiquitous' mean?", "everywhere", {}, -1, "Present, appearing, or found everywhere.", "Medium"});
    allTasks.append({ExerciseType::Translation, "A person who writes books is an ______.", "author", {}, -1, "Like J.K. Rowling.", "Medium"});
    allTasks.append({ExerciseType::Translation, "What is a 'culinary' art related to?", "cooking", {}, -1, "Pertaining to the kitchen or cookery.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Opposite of 'generous'?", "selfish", {}, -1, "Concerned only with oneself.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'beautiful'?", "gorgeous", {}, -1, "Extremely pretty or attractive.", "Medium"});
    allTasks.append({ExerciseType::Translation, "A journey for pleasure is often called a ______.", "vacation", {}, -1, "Or holiday.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Complete: The detective tried to ______ the mystery.", "solve", {}, -1, "Find an answer or explanation.", "Medium"});
    allTasks.append({ExerciseType::Translation, "What is 'chronological' order?", "time", {}, -1, "Arranged in order of time.", "Medium"});
    allTasks.append({ExerciseType::Translation, "To 'hesitate' means to ______.", "pause", {}, -1, "Stop briefly before doing something.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Antonym of 'ancient'?", "modern", {}, -1, "Relating to present times.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Someone who is 'optimistic' expects good things to ______.", "happen", {}, -1, "Positive outlook.", "Medium"});
    allTasks.append({ExerciseType::Translation, "A 'nocturnal' animal is active at ______.", "night", {}, -1, "Opposite of diurnal.", "Medium"});
    allTasks.append({ExerciseType::Translation, "What does 'diligent' mean?", "hard-working", {}, -1, "Showing care and conscientiousness in one's work.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'error'?", "mistake", {}, -1, "Something done incorrectly.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Antonym of 'victory'?", "defeat", {}, -1, "Losing a contest or battle.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Complete: The old castle was ______ and mysterious.", "ancient", {}, -1, "Belonging to the very distant past.", "Medium"});
    allTasks.append({ExerciseType::Translation, "To 'innovate' means to introduce new ______ or methods.", "ideas", {}, -1, "Make changes by introducing new things.", "Medium"});
    allTasks.append({ExerciseType::Translation, "What is a 'verbose' speaker like?", "talkative", {}, -1, "Using more words than needed.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Opposite of 'fragile'?", "durable", {}, -1, "Able to withstand wear or damage.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'adequate'?", "sufficient", {}, -1, "Enough or satisfactory for a particular purpose.", "Medium"});
    allTasks.append({ExerciseType::Translation, "A 'hypothesis' is a proposed ______ based on limited evidence.", "explanation", {}, -1, "A starting point for further investigation.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Complete: She showed great ______ in the face of danger.", "courage", {}, -1, "Bravery.", "Medium"});
    allTasks.append({ExerciseType::Translation, "What does 'mandatory' mean?", "compulsory", {}, -1, "Required by law or rules.", "Medium"});
    allTasks.append({ExerciseType::Translation, "A 'summary' is a brief statement of the main ______.", "points", {}, -1, "A short overview.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Antonym of 'temporary'?", "permanent", {}, -1, "Lasting or intended to last forever.", "Medium"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'assist'?", "help", {}, -1, "To give support or aid.", "Medium"});
    allTasks.append({ExerciseType::Translation, "To 'comprehend' something is to ______ it.", "understand", {}, -1, "Grasp mentally.", "Medium"});

    // ==========================================
    // MEDIUM - Grammar (ExerciseType::Grammar)
    // ==========================================
    allTasks.append({ExerciseType::Grammar, "She ______ (go) to the store yesterday.", "", {"goes", "went", "is going", "has gone"}, 1, "Past simple tense for a completed action in the past.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "If I ______ (be) you, I would study more.", "", {"am", "was", "were", "is"}, 2, "Subjunctive mood for hypothetical situations.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "They ______ already finished their homework.", "", {"have", "has", "had", "are"}, 0, "Present perfect tense with 'they'.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "This book is ______ than that one.", "", {"good", "better", "best"}, 1, "Comparative form of 'good'.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "He is interested ______ learning Spanish.", "", {"in", "on", "at", "for"}, 0, "Correct preposition with 'interested'.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "Neither my brother nor my parents ______ here.", "", {"is", "are", "was"}, 1, "Verb agrees with the closest subject ('parents').", "Medium"});
    allTasks.append({ExerciseType::Grammar, "I haven't seen him ______ last year.", "", {"since", "for", "from"}, 0, "Use 'since' with a specific point in time.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "She is the ______ (tall) girl in the class.", "", {"tall", "taller", "tallest"}, 2, "Superlative form.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "By this time tomorrow, we ______ (arrive) in Paris.", "", {"will arrive", "will have arrived", "arrived"}, 1, "Future perfect tense.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "How ______ money do you have?", "", {"much", "many", "lot"}, 0, "For uncountable nouns like 'money'.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "The news ______ surprisingly good.", "", {"is", "are", "were"}, 0, "'News' is an uncountable noun, takes singular verb.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "Can you tell me where ______?", "", {"is the station", "the station is", "is station"}, 1, "Embedded question word order.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "She insisted ______ paying for the meal.", "", {"on", "in", "to", "about"}, 0, "Correct preposition with 'insisted'.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "It's important ______ (be) on time.", "", {"be", "to be", "being"}, 1, "Infinitive with 'to' after 'important'.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "Who ______ this cake?", "", {"made", "make", "making"}, 0, "Past simple for a completed action.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "While I ______ (watch) TV, the phone rang.", "", {"watched", "was watching", "am watching"}, 1, "Past continuous for an ongoing action interrupted.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "This is the house ______ I was born.", "", {"which", "where", "who"}, 1, "Relative pronoun for place.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "You ______ (should) apologize for your behavior.", "", {"should", "must", "can"}, 0, "Modal verb for advice.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "I'm looking forward ______ (see) you.", "", {"to see", "to seeing", "seeing"}, 1, "'Looking forward to' + gerund.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "He speaks English ______ (fluent).", "", {"fluent", "fluently", "fluency"}, 1, "Adverb form to describe how he speaks.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "If it ______ (rain) tomorrow, we will stay home.", "", {"rain", "rains", "will rain"}, 1, "First conditional, present simple in if-clause.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "There isn't ______ milk left in the fridge.", "", {"some", "any", "no"}, 1, "Use 'any' in negative sentences.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "The car ______ (steal) last night.", "", {"stole", "was stolen", "has stolen"}, 1, "Passive voice, past simple.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "She is ______ than her sister.", "", {"more smart", "smarter", "smartest"}, 1, "Comparative adjective.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "I've lived here ______ ten years.", "", {"since", "for", "ago"}, 1, "Use 'for' with a period of time.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "He asked me ______ I wanted coffee.", "", {"if", "that", "did"}, 0, "Reported question.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "Each of the students ______ a textbook.", "", {"has", "have", "are having"}, 0, "'Each' is singular.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "We decided ______ (go) to the cinema.", "", {"go", "to go", "going"}, 1, "Verb + to-infinitive.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "This coffee is too hot ______ (drink).", "", {"drink", "to drink", "drinking"}, 1, "'Too ... to' structure.", "Medium"});
    allTasks.append({ExerciseType::Grammar, "My keys ______ (be) on the table a minute ago.", "", {"was", "were", "is"}, 1, "'Keys' is plural.", "Medium"});


    // ==========================================
    // HARD - Translation/Vocabulary (ExerciseType::Translation)
    // ==========================================
    allTasks.append({ExerciseType::Translation, "What does 'ephemeral' mean?", "short-lived", {}, -1, "Lasting for a very short time.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'mitigate'?", "alleviate", {}, -1, "To make less severe or painful.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Antonym for 'ubiquitous'?", "rare", {}, -1, "Not common; very infrequent.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Complete: The politician's speech was filled with ______ promises.", "grandiose", {}, -1, "Impressive but impractical or unrealistically large.", "Hard"});
    allTasks.append({ExerciseType::Translation, "What is a 'panacea'?", "cure-all", {}, -1, "A solution or remedy for all difficulties or diseases.", "Hard"});
    allTasks.append({ExerciseType::Translation, "To 'exacerbate' a problem is to make it ______.", "worse", {}, -1, "Increase the severity.", "Hard"});
    allTasks.append({ExerciseType::Translation, "A 'cacophony' is a harsh, discordant mixture of ______.", "sounds", {}, -1, "Often unpleasant to hear.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Someone who is 'laconic' uses very few ______.", "words", {}, -1, "Brief and to the point.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'surreptitious'?", "secretive", {}, -1, "Kept secret, especially because it would not be approved of.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Antonym for 'benevolent'?", "malevolent", {}, -1, "Having or showing a wish to do evil to others.", "Hard"});
    allTasks.append({ExerciseType::Translation, "What does 'ameliorate' mean?", "improve", {}, -1, "Make (something bad or unsatisfactory) better.", "Hard"});
    allTasks.append({ExerciseType::Translation, "A 'paragon' is a model of ______.", "excellence", {}, -1, "A person or thing regarded as a perfect example.", "Hard"});
    allTasks.append({ExerciseType::Translation, "To be 'gregarious' is to be ______.", "sociable", {}, -1, "Fond of company.", "Hard"});
    allTasks.append({ExerciseType::Translation, "What is an 'enigma'?", "puzzle", {}, -1, "A person or thing that is mysterious or difficult to understand.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'fastidious'?", "meticulous", {}, -1, "Very attentive to and concerned about accuracy and detail.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Antonym for 'ostentatious'?", "modest", {}, -1, "Not pretentious or showy.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Complete: His ______ arguments failed to convince the jury.", "tenuous", {}, -1, "Very weak or slight.", "Hard"});
    allTasks.append({ExerciseType::Translation, "What does 'vicarious' mean (e.g., vicarious pleasure)?", "indirect", {}, -1, "Experienced through the feelings or actions of another person.", "Hard"});
    allTasks.append({ExerciseType::Translation, "A 'neophyte' is a ______ to a subject or activity.", "beginner", {}, -1, "A novice.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'pernicious'?", "harmful", {}, -1, "Having a gradually damaging effect in a subtle way.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Antonym for 'taciturn'?", "talkative", {}, -1, "Reserved or uncommunicative in speech.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Complete: The scientist's ______ theory was later proven correct.", "prescient", {}, -1, "Having or showing knowledge of events before they take place.", "Hard"});
    allTasks.append({ExerciseType::Translation, "What does 'pulchritudinous' mean?", "beautiful", {}, -1, "Breathtakingly beautiful (often humorous or formal).", "Hard"});
    allTasks.append({ExerciseType::Translation, "To 'obfuscate' something is to make it ______.", "unclear", {}, -1, "To render obscure or unintelligible.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Synonym for 'querulous'?", "complaining", {}, -1, "Peevish, whining.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Antonym for 'sagacious'?", "foolish", {}, -1, "Having or showing keen mental discernment and good judgment.", "Hard"});
    allTasks.append({ExerciseType::Translation, "Complete: The evidence was ______ , offering little support.", "scant", {}, -1, "Barely sufficient or adequate.", "Hard"});
    allTasks.append({ExerciseType::Translation, "What does 'salubrious' mean?", "health-giving", {}, -1, "Favorable to or promoting health; healthful.", "Hard"});
    allTasks.append({ExerciseType::Translation, "A 'sycophant' is a person who acts ______ toward someone important.", "obsequiously", {}, -1, "A flatterer or yes-man.", "Hard"});

    // ==========================================
    // HARD - Grammar (ExerciseType::Grammar)
    // ==========================================
    allTasks.append({ExerciseType::Grammar, "Had I known you were coming, I ______ (bake) a cake.", "", {"would bake", "would have baked", "baked", "had baked"}, 1, "Third conditional (past unreal condition).", "Hard"});
    allTasks.append({ExerciseType::Grammar, "Not only ______ (he arrive) late, but he also forgot his presentation.", "", {"he arrived", "did he arrive", "he had arrived", "arrived he"}, 1, "Inversion after 'Not only'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "It's high time you ______ (start) taking this seriously.", "", {"start", "started", "to start", "should start"}, 1, "Past subjunctive after 'It's high time'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "The committee ______ unable to agree on a solution.", "", {"is", "are", "was", "were"}, 1, "Committee treated as individuals (disagreement implies plural). Use 'was' if treated as a single unit.", "Hard"}); // 'were' is also common for individual disagreement.
    allTasks.append({ExerciseType::Grammar, "She expressed a desire ______ the manager.", "", {"to see", "seeing", "see", "to have seen"}, 0, "Infinitive after 'desire'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "Despite ______ (feel) unwell, he went to work.", "", {"feeling", "to feel", "felt", "he felt"}, 0, "Gerund after 'despite'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "I'd rather you ______ (not tell) anyone about this.", "", {"don't tell", "didn't tell", "not tell", "won't tell"}, 1, "Subjunctive after 'would rather someone'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "The data ______ (suggest) a different conclusion. (plural interpretation)", "", {"suggest", "suggests", "is suggesting", "are suggesting"}, 0, "'Data' often treated as plural, especially in formal/scientific contexts.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "There are fewer ______ (student) in this class than last year.", "", {"student", "students", "student's"}, 1, "Plural countable noun after 'fewer'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "He is one of those people who ______ always complaining.", "", {"is", "are", "was", "has"}, 1, "Verb agrees with 'people' (antecedent of 'who').", "Hard"});
    allTasks.append({ExerciseType::Grammar, "The CEO, along with the board members, ______ (be) present at yesterday's meeting.", "", {"is", "are", "was", "were"}, 2, "Subject is CEO (singular); 'along with' phrase doesn't change number. Past context.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "No sooner ______ (the sun rise) than the birds began to sing.", "", {"the sun rose", "did the sun rise", "had the sun risen", "sun rised"}, 2, "Inversion with 'No sooner...than', past perfect needed.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "I wish I ______ (can) speak French fluently.", "", {"can", "could", "can to", "be able to"}, 1, "Subjunctive after 'wish' for present unreal ability.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "The number of accidents ______ decreased this year.", "", {"has", "have", "is", "are"}, 0, "'The number' is singular.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "She is accustomed ______ (work) long hours.", "", {"to work", "to working", "working", "work"}, 1, "'Accustomed to' + gerund.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "Little ______ (he know) about the surprise party.", "", {"he knew", "did he know", "knew he", "he had known"}, 1, "Inversion after negative adverbial 'Little'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "It is imperative that he ______ (attend) the meeting.", "", {"attends", "attend", "to attend", "is attending"}, 1, "Subjunctive mood after 'imperative that'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "She wondered whether ______ (go) or stay.", "", {"go", "to go", "going", "should go"}, 1, "Infinitive or 'should go' in reported decisions/dilemmas.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "My preference is ______ (read) a book rather than watch TV.", "", {"reading", "to read", "read", "for reading"}, 1, "'Preference is to read' or 'Preference for reading'. 'to read' fits here.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "All the furniture in this room ______ (be) antique.", "", {"is", "are", "was", "were"}, 0, "'Furniture' is uncountable, takes a singular verb.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "The criteria for selection ______ (be) very strict.", "", {"is", "are", "was", "were"}, 1, "'Criteria' is the plural of 'criterion'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "I suggest that she ______ (consult) a specialist.", "", {"consults", "consult", "to consult", "is consulting"}, 1, "Subjunctive after 'suggest that'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "They objected ______ (be) treated unfairly.", "", {"to be", "to being", "being", "be"}, 1, "'Objected to' + gerund.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "Rarely ______ (one see) such a beautiful sunset.", "", {"one sees", "does one see", "sees one", "one has seen"}, 1, "Inversion after 'Rarely'.", "Hard"});
    allTasks.append({ExerciseType::Grammar, "He denied ______ (take) the money.", "", {"to take", "taking", "to have taken", "having taken"}, 1, "'Denied' + gerund (or perfect gerund for past action). 'taking' is simpler.", "Hard"});


    qInfo() << "Total hardcoded tasks defined:" << allTasks.size();
    return allTasks;
}

static QMap<QString, QMap<ExerciseType, QList<TaskData>>> categorizedTasksCache;

inline void ensureTasksCategorized() {
    if (categorizedTasksCache.isEmpty()) {
        qInfo() << "Categorizing all hardcoded tasks into cache...";
        QList<TaskData> allTasks = getAllHardcodedTasks();
        for (const TaskData& task : allTasks) {
            categorizedTasksCache[task.originalDifficulty][task.type].append(task);
        }

        for (auto diff_it = categorizedTasksCache.constBegin(); diff_it != categorizedTasksCache.constEnd(); ++diff_it) {
            for (auto type_it = diff_it.value().constBegin(); type_it != diff_it.value().constEnd(); ++type_it) {
                qInfo() << "Cached:" << diff_it.key() << (type_it.key() == ExerciseType::Grammar ? "Grammar" : "Translation/Vocab")
                << "Tasks:" << type_it.value().size();
            }
        }
    }
}

inline QList<TaskData> getSampleTasks(ExerciseType type, const QString& difficulty, int N_tasksToSelect) {
    ensureTasksCategorized();

    QList<TaskData> availableTasks;

    if (categorizedTasksCache.contains(difficulty) &&
        categorizedTasksCache[difficulty].contains(type)) {
        availableTasks = categorizedTasksCache[difficulty][type];
    } else {
        qWarning() << "No tasks found in cache for difficulty:" << difficulty
                   << "and type:" << (type == ExerciseType::Grammar ? "Grammar" : "Translation/Vocab");
        if (difficulty != "Medium" && categorizedTasksCache.contains("Medium") && categorizedTasksCache["Medium"].contains(type)) {
            qInfo() << "Falling back to Medium difficulty for task selection.";
            availableTasks = categorizedTasksCache["Medium"][type];
        } else {
            return {};
        }
    }

    if (availableTasks.isEmpty()) {
        qWarning() << "Task list is empty for difficulty:" << difficulty
                   << "and type:" << (type == ExerciseType::Grammar ? "Grammar" : "Translation/Vocab");
        return {};
    }

    std::shuffle(availableTasks.begin(), availableTasks.end(), *QRandomGenerator::global());

    QList<TaskData> selectedTasks;
    for (int i = 0; i < qMin(N_tasksToSelect, availableTasks.size()); ++i) {
        selectedTasks.append(availableTasks[i]);
    }

    if (selectedTasks.size() < N_tasksToSelect) {
        qWarning() << "Could only select" << selectedTasks.size() << "tasks of type"
                   << (type == ExerciseType::Grammar ? "Grammar" : "Translation/Vocab")
                   << "for difficulty" << difficulty << "(wanted" << N_tasksToSelect << ")";
    }

    return selectedTasks;
}


#endif
